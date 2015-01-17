#ifndef MEMMAPAREA_HH_INCLUDED
#define MEMMAPAREA_HH_INCLUDED

#include "kernel/arch/current/virtaddr.hh"
#include "kernel/fs/filelike.hh"

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
		VirtAddr zeroStart;
		size_t offset;

		MemMapArea *parent, *lower, *upper;
		FileLike *file;
		void *priv;
		Prot prot;

        public:
		MemMapArea(VirtAddr b = VirtAddr((void*)0), VirtAddr e = VirtAddr((void*)0)) :
			base(b), end(e), zeroStart(e), offset(0), parent(0), lower(0), upper(0), file(0), priv(0), prot((Prot)0) { }

		void adjust(VirtAddr newBase) {
			base = newBase;
			uint32_t delta = newBase - VirtAddr(0);
			end = end + delta;
			zeroStart = zeroStart + delta;
		}

		void setBase(VirtAddr b)       { base = b; }
		void setEnd (VirtAddr e)       { end  = e; zeroStart = e; }
		void setZeroStart(VirtAddr s)  { zeroStart = s; }
		void setProt(Prot p)           { prot = p; }
		void setPrivPointer(void *p)   { priv = p; }
		void setOffset(size_t o)       { offset = o; }
		void setFile(FileLike *f)      { file = f; }

		void *getPrivPointer() const     { return priv; }
		size_t getOffset() const         { return offset; }
		Prot getProt() const             { return prot; }
		VirtAddr    getBase()      const { return base; }
		VirtAddr    getEnd()       const { return end; }
		VirtAddr    getZeroStart() const { return zeroStart; }
		PageLoader *getLoader()    const { return loader; }
		FileLike   *getFile()	   const { return file; }

		void setLoader(PageLoader *l)   { loader = l; }

		bool contains(VirtAddr v) const {
			return (v >= base && v < end);
		}

		friend class MemMap;
};

#endif

