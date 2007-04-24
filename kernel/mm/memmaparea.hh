#ifndef MEMMAPAREA_HH_INCLUDED
#define MEMMAPAREA_HH_INCLUDED

#include "kernel/arch/current/virtaddr.hh"

class PageLoader; 
class MemMapArea {
	public:
		enum Prot {
			NONE = 0,
			X    = 1,
			W    = 2,
			WX   = 3,
			R    = 4,
			RX   = 5,
			RW   = 6,
			RWX  = 7
		};
	private:
		PageLoader *loader;
		VirtAddr base;
		VirtAddr end;
		MemMapArea *parent, *lower, *upper;
		void *priv;
		Prot prot;
        public:
		MemMapArea(VirtAddr b = VirtAddr((void*)0), VirtAddr e = VirtAddr((void*)0)) :
			base(b), end(e), parent(0), lower(0), upper(0), prot((Prot)0) { }

		void setBase(VirtAddr b) { base = b; }
		void setEnd (VirtAddr e) { end  = e; }
		void setProt(Prot p)     { prot = p; } 		
		void setPrivPointer(void *p) { priv = p; } 		
		void *getPrivPointer()       { return priv; } 		
		
		Prot getProt()           { return prot; } 		

		VirtAddr    getBase()   const { return base; }
		VirtAddr    getEnd()    const { return end; } 
		PageLoader *getLoader() const { return loader; }
		
		void setLoader(PageLoader *l)   { loader = l; }

		bool contains(VirtAddr v) const { 
			return (v >= base && v < end);
		}

		friend class MemMap;
};

#endif

