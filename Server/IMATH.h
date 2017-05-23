//
//
//
#include <Unknwn.h>
#include <initguid.h>

DEFINE_GUID(CLSID_Math,
	0x3030c27, 0x753f, 0x4374, 0x9a, 0xb, 0xc1, 0x90, 0xd1, 0x1f, 0x56, 0xc1);

DEFINE_GUID(IID_IMath,
	0xd7ab8963, 0x4081, 0x42cb, 0x96, 0xb1, 0x44, 0x41, 0x14, 0xb, 0x3d, 0x37);

DEFINE_GUID(IID_IAdvancedMath,
	0x249de54, 0x6989, 0x47ec, 0x80, 0x71, 0xc3, 0xb4, 0xdf, 0xbf, 0xe1, 0xd3);

class IMath : public IUnknown
{
public:
	STDMETHOD(Add)(long, long, long*) PURE;
	STDMETHOD(Subtract)(long, long, long*) PURE;
	STDMETHOD(Multiply)(long, long, long*) PURE;
	STDMETHOD(Divide)(long, long, long*) PURE;
};

class IAdvanceMath : public IUnknown
{
public:
	STDMETHOD(Factorial)(short, long*) PURE;
	STDMETHOD(Fibonacci)(short, long*) PURE;
};