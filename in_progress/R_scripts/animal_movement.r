# # setwd("C:/Weaver/Weaver-main4/Weaver-main/result_simulation/base_dinos_Joan_20/animals_each_day_end")
setwd("/home/macarse/Escritorio/EEZA/Weaver/result_simulation/base_dinos_Joan_1/animals_each_day_start")

library(ggplot2)
library(vegan)
library(spatstat)
library(spatial)

interpol2<-function(x,minx,maxx,miny,maxy)  ##interpolation for values
{
	y=miny+((x-minx)*((maxy-miny)/(maxx-minx)))
	return(y)
}

output="absolute" ##"absolute" "relative" 

xmin=0
xmax=9
ymin=0
ymax=9

length(dir())

for(i in 1:300){

	for(j in 1:1){  ##100000000
	}

	if(i==1){

		file<-read.table(dir()[i],h=T) ##Edmontosaurus_sp  Tyrannosaurus_sp
		edmos<-file[file$species %in% "Edmontosaurus_sp",]
		# plot(edmos$x,edmos$y,pch=16,col="blue",xlim=c(0,8),
		# ylim=c(0,98))
		# Trex<-file[file$species %in% "Tyrannosaurus_sp",]
		# points(Trex$x,Trex$y,pch=16,col="red",cex=4)

		agg<-aggregate(fecundity~x+y+species,data=file,length)
		
		if(output=="relative"){
			maxForInterpol=max(agg$fecundity)
			minForInterpol=min(agg$fecundity)
			agg$fecundity<-interpol2(agg$fecundity,minForInterpol,maxForInterpol,0,1)
		}
		
		g1<-ggplot(agg, aes(x=agg$x, y=agg$y, 
		size=agg$fecundity, color=species)) +
		  geom_point(alpha=0.6)  +
		  scale_size(range=c(2, 10))  + 
		  xlim(xmin, xmax)+ 
		  ylim(ymin, ymax) 

		print(g1)
		
				
		file<-file[file$species %in% "Edmontosaurus_sp",]
		
		forppp<-list()
		forppp$x=file$x+runif(length(file$x))
		forppp$y=file$y+runif(length(file$y))
		forppp$area=c(xl=xmin,xu=xmax+0.99,yl=ymin,yu=ymax+0.99)
		agg_ppp<-as.ppp(forppp)
		print(clarkevans(agg_ppp))

		
		agg<-agg[agg$species %in% "Edmontosaurus_sp",]
        agg_mat<-as.matrix(data.frame(abundance=agg$fecundity))
        print(dispindmorisita(agg_mat))



	}else{
	
		file<-read.table(dir()[i],h=T)
		# edmos<-file[file$species %in% "Edmontosaurus_sp",]
		# plot(edmos$x,edmos$y,pch=16,col="blue",xlim=c(0,8),
		# ylim=c(0,98))
    	# Trex<-file[file$species %in% "Tyrannosaurus_sp",]
		# points(Trex$x,Trex$y,pch=16,col="red",cex=4)

		agg<-aggregate(fecundity~x+y+species,data=file,length)

		if(output=="relative"){
			maxForInterpol=max(agg$fecundity)
			minForInterpol=min(agg$fecundity)
			agg$fecundity<-interpol2(agg$fecundity,minForInterpol,maxForInterpol,0,1)
		}
		
		g1<-ggplot(agg, aes(x=agg$x, y=agg$y, 
		size=agg$fecundity, color=species)) +
		  geom_point(alpha=0.6)  +
		  scale_size(range=c(2, 10))  + 
		  xlim(xmin, xmax) + 
		  ylim(ymin, ymax) 
		  
		print(g1)
		
		
		file<-file[file$species %in% "Edmontosaurus_sp",]
		
		forppp<-list()
		forppp$x=file$x+runif(length(file$x))
		forppp$y=file$y+runif(length(file$y))
		forppp$area=c(xl=xmin,xu=xmax+0.99,yl=ymin,yu=ymax+0.99)
		agg_ppp<-as.ppp(forppp)
		print(clarkevans(agg_ppp))

		
		agg<-agg[agg$species %in% "Edmontosaurus_sp",]
        agg_mat<-as.matrix(data.frame(abundance=agg$fecundity))
        print(dispindmorisita(agg_mat))
		
		



	}

}





forppp<-list()
forppp$x=file$x+runif(length(file$x))
forppp$y=file$y+runif(length(file$y))
forppp$area=c(xl=xmin,xu=xmax+0.99,yl=ymin,yu=ymax+0.99)
agg_ppp<-as.ppp(forppp)
print(clarkevans(agg_ppp))

miplot(agg_ppp)


agg<-agg[agg$species %in% "Edmontosaurus_sp",]
agg_mat<-as.matrix(data.frame(abundance=agg$fecundity))
agg_mat

dispindmorisita(agg_mat)


              imor     mclu      muni      imst      pchisq
abundance 1.080091 1.057763 0.9504407 0.5001397 0.004488801

##going for resources...
> dispindmorisita(agg_mat)
              imor     mclu      muni      imst       pchisq
abundance 1.110728 1.057848 0.9503206 0.5003267 0.0002822276


i=1
file<-read.table(dir()[i],h=T)
agg<-aggregate(fecundity~x+y+species,data=file,length)
agg<-agg[agg$species %in% "Edmontosaurus_sp",]
agg_mat<-as.matrix(data.frame(abundance=agg$fecundity))
agg_mat

dispindmorisita(agg_mat)



###conspecifics at 1... others at 0
i=2
file<-read.table(dir()[i],h=T) ##Edmontosaurus_sp  Tyrannosaurus_sp
edmos<-file[file$species %in% "Edmontosaurus_sp",]
table(edmos$x,edmos$y)


      0   1   2   3   4   5   6   7   8
  0   0   0   0   0  71   0   0   0   0
  1   0   0  66  74  94  81  68   0   0
  2   0  87  92  88  76  90  75 102   0
  3   0  83  78  76  77  81  80  92   0
  4  80  84  87 110  87  86  79  69  80
  5   0  81  91 103  69  80  86  88   0
  6   0  85  87  67  76  79  91  86   0
  7   0   0  82  87  84  99  73   0   0
  8   0   0   0   0  81   0   0   0   0



i=49
table(edmos$x,edmos$y)
     0  1  2  3  4  5  6  7  8  9
  0 18 21 26 30 29 23 21 32 20 14
  1 20 36 40 46 46 43 43 32 32 15
  2 19 32 49 43 50 67 56 53 37 34
  3 26 44 45 45 52 46 52 48 28 29
  4 20 45 61 60 59 61 46 53 52 36
  5 32 38 70 62 63 53 65 52 53 31
  6 33 57 59 65 71 55 57 40 46 25
  7 24 52 52 65 61 46 53 63 51 36
  8 19 31 54 45 45 40 39 45 26 19
  9 14 28 14 25 26 25 25 27 19 11



###biomass at 1... others at 0
i=1
table(edmos$x,edmos$y)
      0   1   2   3   4   5   6   7   8
  0   0   0   0   0  71   0   0   0   0
  1   0   0  66  74  94  81  68   0   0
  2   0  87  92  88  76  90  75 102   0
  3   0  83  78  76  77  81  80  92   0
  4  80  84  87 110  87  86  79  69  80
  5   0  81  91 103  69  80  86  88   0
  6   0  85  87  67  76  79  91  86   0
  7   0   0  82  87  84  99  73   0   0
  8   0   0   0   0  81   0   0   0   0




i=49
table(edmos$x,edmos$y)
       0    1    2    3    4    5    6    7    8
  0    0    0    0    0    3    0    0    0    0
  1    0    0   70   12    2   10    2    0    0
  2    0  357    9   16    3    7    7    7    0
  3    0 1360   16   13    7    5   15    6    0
  4  635   10    4    9    6   10    7    0    2
  5    0 1229   12   27   11    7    7    0    0
  6    0   15   12    5    3    8    3   10    0
  7    0    0    2    7    8   15   12    0    0
  8    0    0    0    0   15    0    0    0    0




###anticonspecifics at 1... others at 0
i=1
table(edmos$x,edmos$y)
      0   1   2   3   4   5   6   7   8
  0   0   0   0   0  71   0   0   0   0
  1   0   0  66  74  94  81  68   0   0
  2   0  87  92  88  76  90  75 102   0
  3   0  83  78  76  77  81  80  92   0
  4  80  84  87 110  87  86  79  69  80
  5   0  81  91 103  69  80  86  88   0
  6   0  85  87  67  76  79  91  86   0
  7   0   0  82  87  84  99  73   0   0
  8   0   0   0   0  81   0   0   0   0





i=49
table(edmos$x,edmos$y)
     0  1  2  3  4  5  6  7  8  9
  0 18 21 26 30 29 23 21 32 20 14
  1 20 36 40 46 46 43 43 32 32 15
  2 19 32 49 43 50 67 56 53 37 34
  3 26 44 45 45 52 46 52 48 28 29
  4 20 45 61 60 59 61 46 53 52 36
  5 32 38 70 62 63 53 65 52 53 31
  6 33 57 59 65 71 55 57 40 46 25
  7 24 52 52 65 61 46 53 63 51 36
  8 19 31 54 45 45 40 39 45 26 19
  9 14 28 14 25 26 25 25 27 19 11






###antipredatory at 1... others at 0
i=1
table(edmos$x,edmos$y)
      0   1   2   3   4   5   6   7   8
  0   0   0   0   0  71   0   0   0   0
  1   0   0  66  74  94  81  68   0   0
  2   0  87  92  88  76  90  75 102   0
  3   0  83  78  76  77  81  80  92   0
  4  80  84  87 110  87  86  79  69  80
  5   0  81  91 103  69  80  86  88   0
  6   0  85  87  67  76  79  91  86   0
  7   0   0  82  87  84  99  73   0   0
  8   0   0   0   0  81   0   0   0   0





i=49
table(edmos$x,edmos$y)
     0  1  2  3  4  5  6  7  8  9
  0 18 21 26 30 29 23 21 32 20 14
  1 20 36 40 46 46 43 43 32 32 15
  2 19 32 49 43 50 67 56 53 37 34
  3 26 44 45 45 52 46 52 48 28 29
  4 20 45 61 60 59 61 46 53 52 36
  5 32 38 70 62 63 53 65 52 53 31
  6 33 57 59 65 71 55 57 40 46 25
  7 24 52 52 65 61 46 53 63 51 36
  8 19 31 54 45 45 40 39 45 26 19
  9 14 28 14 25 26 25 25 27 19 11



