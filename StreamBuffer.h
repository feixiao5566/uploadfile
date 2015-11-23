#ifndef    _ZJY_STREAMBUFFER_H_
#define    _ZJY_STREAMBUFFER_H_
#include <string>
//本篇是从网络读函数和向网络写函数,主要用<<和>>重载符号
class CReadStream
{
public:
	CReadStream(const char *buffer,int len)
		:m_bytes(buffer),m_limit(len),m_pos(0)  //CReadStream类构造函数, 初始化列表
	{
	}

	size_t LeftBytes()  //习惯封装
	{
		return m_limit- m_pos; //总限制 - 当前位置 = 剩余
	}

	void Skip(int step)
	{
		m_pos+=step;  //一次跳一步
	}

	const char *data()
	{
		return m_bytes;
	}

/*这个函数很有意思啊
operator>>()读入字符串使用C++标准库，实现读入一个(长度未知的)字符串的代码非常简单,
只需声明一个std::string str;，然后cin>>str;
cin
#include <iostream>  
#include <string>  
using namespace std;  
  
// 略...  
    string str;  
    cin >> str; 


*/
//所以它的作用是,从网络字节序v读入整个v长度的字节,将它转为主机字节顺序,并且返回转换的序列
	CReadStream &operator >>(int &v) //operator>>()读入字符串//C++标准库读入一个长度未知的字符串非常简单,\
	                                  只需声明一个std::string str;，然后cin>>str;
	{
		Get(&v,sizeof(v));    //v是光标,sizeof(v)是长度. 读取n长一段buffer内容,然后光标向后跳转n
		v= ntohl(v);  //将一个无符号长整形数从网络字节顺序转换为主机字节顺序。
		//#include<Winsock2.h> Windows   linux #include <arpa/inet.h>
		return *this;;
	}
//重载
	CReadStream &operator >>(unsigned int &v)
	{
		Get(&v,sizeof(v));
		v= ntohl(v);
		return *this;
	}
//重载
	CReadStream &operator >>(unsigned short &v)
	{
		Get(&v,sizeof(v));
		v= ntohs(v);
		return *this;
	}
/*看不懂惹(⊙o⊙)*/
	CReadStream &operator >>( std::string &str)
	{
		int len=0;
		char c;
		(*this)>>len;  //这是已经在用上面的重载操作符了么?
		while(len--)   //len读取到str的长度了,怎么办到的
		{
			(*this)>>c;  
			str+=c;
		}
		return *this;
	}

	CReadStream &operator >>( char &c)
	{		
		Get(&c,sizeof(char));//读取到原文返回
		return *this;
	}
//这重载的太疯狂..

private:
	void Get(void *data,int n)    //读取n长一段buffer内容,然后光标向后跳转n
	{
		if(m_pos+n>m_limit)
			throw "out of range";
		memcpy(data,m_bytes+m_pos,n);
		m_pos+=n;  
	}
protected:
	const char *m_bytes;
	int m_limit;
	int m_pos;
};

//使用string,可以利用stl的memory pool
class CWriteStream
{
private:
	std::string m_bufer;
public:	
	CWriteStream()
	{
	}

	int size()const
	{
		return (int)m_bufer.size();  //封装的很好,返回m_bufer长度函数
	}

	const char *data()const
	{
		return m_bufer.data();
	}

	CWriteStream &operator <<(const int &v)
	{
		int t=htonl(v);//将主机数转换成无符号长整型的网络字节序.将一个32位数从主机字节顺序转换成网络字节顺序.
		Put(&t,sizeof(t));
		return *this;
	}

	CWriteStream &operator <<(const unsigned int &v)
	{
		int t=htonl(v);
		Put(&t,sizeof(t));
		return *this;
	}

	CWriteStream &operator <<(const unsigned short &v)
	{
		unsigned short t=htons(v);
		Put(&t,sizeof(t));
		return *this;
	}

	//notice!! data not  alias of string
	CWriteStream &operator <<( const char* data)
	{
		size_t len = strlen(data);
		Put(data,(int)len);
		return *this;
	}

	CWriteStream &operator <<( const std::string &str)
	{
		(*this)<<(int)str.size();
		Put(str.c_str(),int(str.size()));
		return *this;
	}

	CWriteStream &operator <<(const char &c)
	{		
		m_bufer+=c;
		return *this;
	}

	CWriteStream &operator <<(const CWriteStream &c)
	{		

		Put( c.data(), c.size() );
		return *this;
	}
	void Put(const void *data,int n)  //把data连接到m_bufer上
	{		
		m_bufer.append((const char*)data,n);//StringBuffer类的函数，用来把后面的字符串接到原来的字符串上面
	}
	void Clear() //清空buffer
	{
		m_bufer.clear();
	}
};
#endif
