#ifndef    _ZJY_STREAMBUFFER_H_
#define    _ZJY_STREAMBUFFER_H_
#include <string>
//��ƪ�Ǵ������������������д����,��Ҫ��<<��>>���ط���
class CReadStream
{
public:
	CReadStream(const char *buffer,int len)
		:m_bytes(buffer),m_limit(len),m_pos(0)  //CReadStream�๹�캯��, ��ʼ���б�
	{
	}

	size_t LeftBytes()  //ϰ�߷�װ
	{
		return m_limit- m_pos; //������ - ��ǰλ�� = ʣ��
	}

	void Skip(int step)
	{
		m_pos+=step;  //һ����һ��
	}

	const char *data()
	{
		return m_bytes;
	}

/*�������������˼��
operator>>()�����ַ���ʹ��C++��׼�⣬ʵ�ֶ���һ��(����δ֪��)�ַ����Ĵ���ǳ���,
ֻ������һ��std::string str;��Ȼ��cin>>str;
cin
#include <iostream>  
#include <string>  
using namespace std;  
  
// ��...  
    string str;  
    cin >> str; 


*/
//��������������,�������ֽ���v��������v���ȵ��ֽ�,����תΪ�����ֽ�˳��,���ҷ���ת��������
	CReadStream &operator >>(int &v) //operator>>()�����ַ���//C++��׼�����һ������δ֪���ַ����ǳ���,\
	                                  ֻ������һ��std::string str;��Ȼ��cin>>str;
	{
		Get(&v,sizeof(v));    //v�ǹ��,sizeof(v)�ǳ���. ��ȡn��һ��buffer����,Ȼ���������תn
		v= ntohl(v);  //��һ���޷��ų��������������ֽ�˳��ת��Ϊ�����ֽ�˳��
		//#include<Winsock2.h> Windows   linux #include <arpa/inet.h>
		return *this;;
	}
//����
	CReadStream &operator >>(unsigned int &v)
	{
		Get(&v,sizeof(v));
		v= ntohl(v);
		return *this;
	}
//����
	CReadStream &operator >>(unsigned short &v)
	{
		Get(&v,sizeof(v));
		v= ntohs(v);
		return *this;
	}
/*��������(��o��)*/
	CReadStream &operator >>( std::string &str)
	{
		int len=0;
		char c;
		(*this)>>len;  //�����Ѿ�������������ز�������ô?
		while(len--)   //len��ȡ��str�ĳ�����,��ô�쵽��
		{
			(*this)>>c;  
			str+=c;
		}
		return *this;
	}

	CReadStream &operator >>( char &c)
	{		
		Get(&c,sizeof(char));//��ȡ��ԭ�ķ���
		return *this;
	}
//�����ص�̫���..

private:
	void Get(void *data,int n)    //��ȡn��һ��buffer����,Ȼ���������תn
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

//ʹ��string,��������stl��memory pool
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
		return (int)m_bufer.size();  //��װ�ĺܺ�,����m_bufer���Ⱥ���
	}

	const char *data()const
	{
		return m_bufer.data();
	}

	CWriteStream &operator <<(const int &v)
	{
		int t=htonl(v);//��������ת�����޷��ų����͵������ֽ���.��һ��32λ���������ֽ�˳��ת���������ֽ�˳��.
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
	void Put(const void *data,int n)  //��data���ӵ�m_bufer��
	{		
		m_bufer.append((const char*)data,n);//StringBuffer��ĺ����������Ѻ�����ַ����ӵ�ԭ�����ַ�������
	}
	void Clear() //���buffer
	{
		m_bufer.clear();
	}
};
#endif
