### R code from vignette source 'shapefiles.Rnw'

###################################################
### code chunk number 1: shapefiles.Rnw:7-8
###################################################
options(SweaveHooks=list(fig=function() par(mar=c(1,1,1,1))))


###################################################
### code chunk number 2: shapefiles.Rnw:25-31
###################################################
library(spatstat)
options(useFancyQuotes=FALSE)
sdate <- read.dcf(file = system.file("DESCRIPTION", package = "spatstat"),
         fields = "Date")
sversion <- read.dcf(file = system.file("DESCRIPTION", package = "spatstat"),
         fields = "Version")


###################################################
### code chunk number 3: shapefiles.Rnw:106-107 (eval = FALSE)
###################################################
## library(maptools)


###################################################
### code chunk number 4: shapefiles.Rnw:111-112 (eval = FALSE)
###################################################
## x <- readShapeSpatial("mydata.shp")


###################################################
### code chunk number 5: shapefiles.Rnw:117-118 (eval = FALSE)
###################################################
## class(x)


###################################################
### code chunk number 6: shapefiles.Rnw:135-139
###################################################
baltim <- columbus <- fylk <- list()
class(baltim) <- "SpatialPointsDataFrame"
class(columbus) <- "SpatialPolygonsDataFrame"
class(fylk) <- "SpatialLinesDataFrame"


###################################################
### code chunk number 7: shapefiles.Rnw:141-145 (eval = FALSE)
###################################################
## setwd(system.file("shapes", package="maptools"))
## baltim   <- readShapeSpatial("baltim.shp")
## columbus <- readShapeSpatial("columbus.shp")
## fylk     <- readShapeSpatial("fylk-val.shp")


###################################################
### code chunk number 8: shapefiles.Rnw:147-150
###################################################
class(baltim)
class(columbus)
class(fylk)


###################################################
### code chunk number 9: shapefiles.Rnw:178-179 (eval = FALSE)
###################################################
## X <- X[W]


###################################################
### code chunk number 10: shapefiles.Rnw:196-197 (eval = FALSE)
###################################################
## y <- as(x, "ppp")


###################################################
### code chunk number 11: shapefiles.Rnw:211-213 (eval = FALSE)
###################################################
## balt <- as(baltim, "ppp")
## bdata <- slot(baltim, "data")


###################################################
### code chunk number 12: shapefiles.Rnw:261-262 (eval = FALSE)
###################################################
## out <- lapply(x@lines, function(z) { lapply(z@Lines, as.psp) })


###################################################
### code chunk number 13: shapefiles.Rnw:271-272 (eval = FALSE)
###################################################
## curvegroup <- lapply(out, function(z) { do.call("superimpose", z)})


###################################################
### code chunk number 14: shapefiles.Rnw:315-319 (eval = FALSE)
###################################################
## out <- lapply(x@lines, function(z) { lapply(z@Lines, as.psp) })
## dat <- x@data
## for(i in seq(nrow(dat))) 
##   out[[i]] <- lapply(out[[i]], "marks<-", value=dat[i, , drop=FALSE])


###################################################
### code chunk number 15: shapefiles.Rnw:340-342
###################################################
getOption("SweaveHooks")[["fig"]]()
data(chorley)
plot(as.owin(chorley), lwd=3, main="polygon")


###################################################
### code chunk number 16: shapefiles.Rnw:355-357
###################################################
getOption("SweaveHooks")[["fig"]]()
data(demopat)
plot(as.owin(demopat), col="blue", main="polygonal region")


###################################################
### code chunk number 17: shapefiles.Rnw:393-396 (eval = FALSE)
###################################################
## regions <- slot(x, "polygons")
## regions <- lapply(regions, function(x) { SpatialPolygons(list(x)) })
## windows <- lapply(regions, as.owin)


###################################################
### code chunk number 18: shapefiles.Rnw:401-402 (eval = FALSE)
###################################################
## te <- tess(tiles=windows)


###################################################
### code chunk number 19: shapefiles.Rnw:438-439 (eval = FALSE)
###################################################
## y <- as(x, "SpatialPolygons")


###################################################
### code chunk number 20: shapefiles.Rnw:449-453 (eval = FALSE)
###################################################
## cp      <- as(columbus, "SpatialPolygons")
## cregions <- slot(cp, "polygons")
## cregions <- lapply(cregions, function(x) { SpatialPolygons(list(x)) })
## cwindows <- lapply(cregions, as.owin)


###################################################
### code chunk number 21: shapefiles.Rnw:463-465 (eval = FALSE)
###################################################
## ch <- hyperframe(window=cwindows)
## ch <- cbind.hyperframe(ch, columbus@data)


###################################################
### code chunk number 22: shapefiles.Rnw:485-487 (eval = FALSE)
###################################################
##   y <- as(x, "im")
##   ylist <- lapply(slot(x, "data"), function(z, y) { y[,] <- z; y }, y=y)


