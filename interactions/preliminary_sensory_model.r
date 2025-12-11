##Exponential decay with extra parameter for stretching
##Arthropods (mg - mm)
###Scope_area
aM=10
bM=0.75

M=seq(0,1000,1)

maxDistance = aM*M^bM

# aDecay=0.01
# bDecay=0.4
# decayRate=aDecay*M^(-bDecay)
# plot(M,decayRate)


senses<-as.data.frame(matrix(nrow=length(M),ncol=3))

names(senses)<-c("M","maxDistance","decayRate")

senses[,1:2]<-cbind(M,maxDistance)

senses<-senses[order(senses$maxDistance,decreasing=T),]


##This below is the solution given by chatGPT to force curves to reach almost zero
eps = 1e-5  # Truncated 0 at maxDistance
beta = 3.5 # Modulates decay rate
senses$decayRate = (-log(eps))^(1/beta)  / (senses$maxDistance+eps)

stretched_expo <- function(d, decayRate) {
  exp( - (decayRate * d)^beta )
}


for(i in seq(1,nrow(senses)-1,100))
{

distances=seq(0,senses$maxDistance[i],senses$maxDistance[i]/100)

	if(i==1){
	plot(distances,stretched_expo(distances,decayRate=senses$decayRate[i]),type="l",lwd=3)
	}else{
	lines(distances,stretched_expo(distances,decayRate=senses$decayRate[i]),col=as.integer(round(runif(1,1,100))),lwd=3)
	}

rm(distances)

}





##Dinosaurs (Kg - Km)
###Scope_area
aM=0.00001
bM=0.75

M=seq(0,22000000,1000)

maxDistance = aM*M^bM

# aDecay=0.01
# bDecay=0.4
# decayRate=aDecay*M^(-bDecay)
# plot(M,decayRate)


senses<-as.data.frame(matrix(nrow=length(M),ncol=3))

names(senses)<-c("M","maxDistance","decayRate")

senses[,1:2]<-cbind(M,maxDistance)

senses<-senses[order(senses$maxDistance,decreasing=T),]


##This below is the solution given by chatGPT to force curves to reach almost zero
eps = 1e-5  # Truncated 0 at maxDistance
beta = 3.5 # Modulates decay rate
senses$decayRate = (-log(eps))^(1/beta)  / (senses$maxDistance+eps)

stretched_expo <- function(d, decayRate) {
  exp( - (decayRate * d)^beta )
}


for(i in seq(1,nrow(senses)-1,1000))
{

distances=seq(0,senses$maxDistance[i],senses$maxDistance[i]/100)

	if(i==1){
	plot(distances,stretched_expo(distances,decayRate=senses$decayRate[i]),type="l",lwd=3)
	}else{
	lines(distances,stretched_expo(distances,decayRate=senses$decayRate[i]),col=as.integer(round(runif(1,1,100))),lwd=3)
	}

rm(distances)

}



