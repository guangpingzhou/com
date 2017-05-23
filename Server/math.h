//
//
//
#include "IMATH.h"

extern long g_lObjs;
extern long g_lLocks;

//����������
class Math : public IMath, public IAdvanceMath
{
public:
	Math();
	~Math();
	//IUnknown
	STDMETHOD(QueryInterface(REFIID, void **));
	STDMETHOD_(ULONG, AddRef());
	STDMETHOD_(ULONG, Release());

	//IMath
	STDMETHOD(Add)(long, long, long *);
	STDMETHOD(Subtract)(long, long, long *);
	STDMETHOD(Multiply)(long, long, long *);
	STDMETHOD(Divide)(long, long, long *);

	//IAdvancedMath
	STDMETHOD(Factorial)(short, long *);
	STDMETHOD(Fibonacci)(short, long *);
protected:
	long m_lRef;
};

//�๤��������
class MathClassFactory : public IClassFactory
{
public:
	MathClassFactory();
	~MathClassFactory();
	
	//IUnknown
	STDMETHOD(QueryInterface(REFIID, void **));
	STDMETHOD_(ULONG, AddRef());
	STDMETHOD_(ULONG, Release());

	//IClassFactory
	STDMETHOD(CreateInstance)(LPUNKNOWN, REFIID, void **);
	STDMETHOD(LockServer)(BOOL);

protected:
	long m_lRef;
};