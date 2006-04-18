C Output from Public domain Ratfor, version 1.0
      subroutine lookup(u,v,ix,x,y,npts,par,period,cifval)
      implicit double precision(a-h,o-z)
      dimension par(5), x(1), y(1), period(2)
      logical per, equisp
      zero = 0.d0
      one = 1.d0
      per = period(1) .gt. zero
      eps = 2.22d-16
      beta = par(1)
      nlook = par(2)
      equisp = par(3) .gt. zero
      delta = par(4)
      rmax = par(5)
      if(npts.eq.0)then
      cifval = beta
      return
      endif
      some = zero
      do23002 j = 1,npts 
      if(j .eq. ix)then
      continue
      else
      if(per)then
      call dist2(u,v,x(j),y(j),period,r2)
      else
      r2 = (u-x(j))**2 + (v-y(j))**2
      endif
      r1 = sqrt(r2)
      if(r1 .ge. rmax)then
      k0 = nlook
      else
      k0 = 1 + int(r1/delta)
      if(equisp)then
      k0 = min(k0,nlook)
      else
      ks = 1 + int(r1/delta)
      rks = par(5+nlook+ks)
      if(rks .le. r1)then
      ksp1 = ks + 1
      do23014 k = ksp1,nlook 
      rk = par(5+nlook+k)
      if(r1 .lt. rk)then
      k0 = k-1
      goto 23015
      endif
23014 continue
23015 continue
      else
      ksm1 = ks - 1
      do23018 k = ksm1,1,-1 
      rk = par(5+nlook+k)
      if(r1 .ge. rk)then
      k0 = k
      goto 23019
      endif
23018 continue
23019 continue
      endif
      endif
      endif
      hk0 = par(5+k0)
      if(hk0 .le. eps)then
      cifval = zero
      return
      endif
      some = some + log(hk0)
      endif
23002 continue
23003 continue
      cifval = beta*exp(some)
      return
      end
