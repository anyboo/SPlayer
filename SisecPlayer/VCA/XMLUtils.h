#ifndef __XML_UTILS_H__
#define __XML_UTILS_H__

#define __USE_MSXML2_NAMESPACE__
#import "msxml3.dll"
//using namespace MSXML2;

//redefine 
#define IXMLDOMNode				MSXML2::IXMLDOMNode
#define IXMLDOMNodePtr			MSXML2::IXMLDOMNodePtr
#define IXMLDOMTextPtr			MSXML2::IXMLDOMTextPtr
#define IXMLDOMElementPtr		MSXML2::IXMLDOMElementPtr
#define IXMLDOMNodeListPtr		MSXML2::IXMLDOMNodeListPtr
#define IXMLDOMAttributePtr		MSXML2::IXMLDOMAttributePtr
#define IXMLDOMDocument2Ptr		MSXML2::IXMLDOMDocument2Ptr

#ifdef _DEBUG
#pragma comment( lib, "comsuppwd" )
#else
#pragma comment( lib, "comsuppw" )
#endif

//Utils XML..
static int GetHex(TCHAR c)
{
	if('0'<=c&&c<='9')return c-'0';
	if('a'<=c&&c<='f')return c-'a'+10;
	if('A'<=c&&c<='F')return c-'A'+10;
	return 0;
}

static int SetHex(int c)
{
	if(0<=c&&c<=9)return c+'0';
	if(10<=c&&c<=15)return c-10+'A';
	return 0;
}

#define for_ChildNodeEnum(a,b)	for(IXMLDOMElementPtr b=a->firstChild;b;b=b->nextSibling)
static DWORD GetAttrValueHex(IXMLDOMElementPtr node,TCHAR* attrName)
{
	DWORD value=0;
	_variant_t v=node->getAttribute(attrName);

	if(v.vt!=VT_NULL){
		_stscanf_s((TCHAR*)_bstr_t(v),_T("%x"),&value);
	}
	return value;
}

static DWORD GetAttrValueNum(IXMLDOMElementPtr node,TCHAR* attrName)
{
	_variant_t v=node->getAttribute(attrName);
	if(v.vt!=VT_NULL){
		return (long)v;
	}
	return 0;

}

static BOOL GetAttrValueString(IXMLDOMElementPtr node,TCHAR* attrName,TCHAR * buffer, DWORD length)
{
	_variant_t v=node->getAttribute(attrName);
	if(v.vt!=VT_NULL){
		_tcscpy_s(buffer, length, (TCHAR*)(_bstr_t(node->getAttribute(attrName))));
		return TRUE;
	}
	return FALSE;
}

static void GetAttrValueBinData(IXMLDOMElementPtr node,TCHAR* attrName,int len, TCHAR* data)
{
	TCHAR * buf=new TCHAR[len*2+1];
	GetAttrValueString(node,attrName,buf,len*2+1);
	for(int i=0;i<len;i++){
		int c1=GetHex(buf[i*2]);
		int c2=GetHex(buf[i*2+1]);
		((BYTE*)data)[i]=c1*16+c2;
	}
	delete buf;
}

static double GetAttrValueFloat(IXMLDOMElementPtr node, LPCTSTR attrName)
{
	_variant_t v=node->getAttribute(attrName);
	if(v.vt!=VT_NULL){
		return (double)v;
	}
	return 0;

}


static void AddWhiteSpaceToNode(IXMLDOMDocument2Ptr pDom, BSTR bstrWs, IXMLDOMNodePtr pNode)
{
    IXMLDOMTextPtr pWs=NULL;
    pWs = pDom->createTextNode(bstrWs);
    pNode->appendChild(pWs);
}


static void SetAttrValueHex(IXMLDOMElementPtr node,TCHAR* attrName,DWORD v)
{
	TCHAR text[32];
	_stprintf_s(text, _countof(text),_T("%x"), v);
	_variant_t value=text;
	node->setAttribute(attrName,value);
}

static void SetAttrValueNum(IXMLDOMElementPtr node,TCHAR* attrName,long v)
{
	_variant_t value(v,VT_I4);
	node->setAttribute(attrName,value);
}

static void SetAttrValueString(IXMLDOMElementPtr node,TCHAR* attrName,TCHAR* buffer)
{
	_variant_t value=buffer;
	node->setAttribute(attrName,value);
}

static void SetAttrValueFloat(IXMLDOMElementPtr node, TCHAR* attrName,double v)
{
	_variant_t value(v,VT_R8);
	node->setAttribute(attrName,value);
}

static void SetAttrValueBinData(IXMLDOMElementPtr node,TCHAR* attrName,int len, TCHAR* data)
{
	
	TCHAR * buf=new TCHAR[len*2+1];
	buf[len*2]=0;

	for(int i=0;i<len;i++){
		int c=((BYTE*)data)[i];
		buf[i*2]=SetHex((c>>4)&0xF);
		buf[i*2+1]=SetHex((c)&0xF);
	}
	SetAttrValueString(node,attrName,buf);
	delete buf;
}


#endif