# 1 "methas.template"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "methas.template"
 ## $Id: methas.template,v 1.5 2008/07/21 23:40:05 adrian Exp adrian $

# 1 "cif.h" 1
  ##
  ## Map from 1 to conditional intensity functions etc
  ##
# 4 "methas.template" 2
# 1 "death.h" 1
# 5 "methas.template" 2

subroutine mh1(par,period,xprop,yprop,mprop,ntypes,
                  iseed,nrep,mrep,p,q,npmax,nverb,x,y,marks,aux,npts,fixall)
implicit double precision(a-h,o-z)



 dimension par(1)

dimension iseed(3)
dimension x(1), y(1), marks(1), period(2)
dimension xprop(1), yprop(1), mprop(1)
integer aux(1)
logical verb, fixall

logical dummyl
integer dummyi

   ## Set up some constants
one = 1.d0
zero = 0.d0
m1 = -1
verb = !(nverb==0)
qnodds = (one - q)/q





   ## Trivial assigments to placate the compiler
dummyi = mprop(1) + ntypes + marks(1)
dummyl = fixall
dummyd = aux(1)
dummyi = dummyi + 1
dummyl = .not. dummyl
dummyd = dummyd * dummyd

   ## Loop:
irep = mrep
while(irep <= nrep) {
 if(verb && mod(irep,nverb)==0) {
  iprt = irep/nverb
  call intpr('irep/nverb=',-1,iprt,1)
 }
 itype = 0
   ## call aru(1,zero,one,iseed,urp)
 call arand(iseed(1),iseed(2),iseed(3),urp)

  ## Shift or birth/death:
 if(urp>p) {
  ## Birth/death:
   ## call aru(1,zero,one,iseed,urq)
  call arand(iseed(1),iseed(2),iseed(3),urq)
  if(urq>q) {

   ## Propose birth:
   u = xprop(irep)
   v = yprop(irep)



   call strauss(u,v,m1,x,y,npts,par,period,cifval)







   anumer = cifval
                        adenom = qnodds*(npts+1)
  ## call aru(1,zero,one,iseed,bp)
   call arand(iseed(1),iseed(2),iseed(3),bp)
   if(bp*adenom < anumer) {
    npts = npts + 1
    itype = 1 ## Birth occurs.
   }
  }
  else {

  ## Propose death:
                       if(npts != 0) {
   ## call aru(1,zero,one,iseed,xi)
    call arand(iseed(1),iseed(2),iseed(3),xi)
    ix = 1 + int(npts*xi)



    call strauss(x(ix),y(ix),ix,x,y,npts,par,period,cifval)







                                anumer = qnodds * npts
    adenom = cifval
   ## call aru(1,zero,one,iseed,dp)
    call arand(iseed(1),iseed(2),iseed(3),dp)
    if(dp*adenom < anumer) itype = 2
                                                       ## Death occurs.
   }
  }
 }
 else {

   ## Propose shift:
               if(npts != 0) {
        ## call aru(1,zero,one,iseed,xi)
                 call arand(iseed(1),iseed(2),iseed(3),xi)
                 ix = 1 + int(npts*xi)
                 u = xprop(irep)
                 v = yprop(irep)
# 134 "methas.template"
                   call strauss(x(ix),y(ix),ix,x,y,npts,par,period,cvd)

                   call strauss(u,v,ix,x,y,npts,par,period,cvn)
# 146 "methas.template"
           ## call aru(1,zero,one,iseed,sp)
                   call arand(iseed(1),iseed(2),iseed(3),sp)
                   if(sp*cvd < cvn) itype = 3 ## Shift occurs.

               }
        }
 if(itype > 0) {



  if(itype==1) { ## Birth
   ix = npts
   x(ix) = u
   y(ix) = v



   if(npts >= npmax) {
   ## Birth has occurred; if there is a birth
   ## on the next repetition, we'll be out of
   ## storage space; go back out to R and
   ## increase the storage space. Note:
   ## There may be no ``next repetition'', i.e.
   ## we ***may*** have just finished the last
   ## (nrep-th) MH step, in which case we squeaked
   ## in with the existing storage space and
   ## we won't actually come back from R.
    mrep = irep+1
    return
   }
  }
  else if(itype==2) { ## Death
   call deathu(x,y,npts,ix)
  }
  else { ## Shift
   x(ix) = u
   y(ix) = v



         }
 }
 irep = irep+1
 mrep = irep
}
return
end
