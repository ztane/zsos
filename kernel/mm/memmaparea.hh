#ifndef MEMMAPAREA_HH_INCLUDED
#define MEMMAPAREA_HH_INCLUDED

class PageLoader; 
class MemMapArea {
	private:
		PageLoader *loader;
		VirtAddr base;
		VirtAddr end;
		MemMapArea lower, upper;
        public:
		MemMapArea(VirtAddr b = 0, VirtAddr e = 0) :
			base(b), end(e), lower(0), upper(0) { }

		void setBase(VirtAddr b) { base = b; }
		void setEnd (VirtAddr e) { end  = e; }
		
		VirtAddr    getBase()   const { return base; }
		VirtAddr    getEnd()    const { return end; } 
		PageLoader *getLoader() const { return loader; }

		friend class MemMap;
};

#endif

