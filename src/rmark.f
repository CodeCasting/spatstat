C Output from Public domain Ratfor, version 1.0
      subroutine rmark(n,ntypes,ptypes,iseed,marks)
      implicit double precision(a-h,o-z)
      dimension iseed(3), ptypes(1), marks(1)
      do23000 i = 1,n 
      call arand(iseed(1),iseed(2),iseed(3),rv)
      psum = 0.d0
      do23002 j = 1,ntypes 
      psum = psum+ptypes(j)
      if(rv .lt. psum)then
      marks(i) = j
      goto 23003
      endif
23002 continue
23003 continue
23000 continue
23001 continue
      return
      end
