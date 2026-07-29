rm(list=ls(all=TRUE))

setwd("C:/Users/jordi/Dropbox/EEZA_2023/Weaver_2023/Encarni_rios/growth_curves")
getwd()

chiro<-read.table("Chironomidae_Cricotopus.txt",h=T)

##Calculate a common lengthAtBirth for all temps
lengthAtBirth = min(chiro$mm)


head(chiro)
  temperature days   mm
1          10 1.58 0.58
2          10 1.82 0.58
3          10 1.63 0.65
4          10 2.14 0.66
5          10 2.76 0.72
6          10 2.70 0.86

tail(chiro)
    temperature  days   mm
174          20 11.21 4.79
175          20 11.16 4.88
176          20 11.17 4.91
177          20 10.95 4.97
178          20 10.89 5.00
179          20 11.34 4.97


names(chiro)<-c("temperature","age","length")

chiro10<-chiro[chiro$temperature == 10,]
chiro15<-chiro[chiro$temperature == 15,]
chiro20<-chiro[chiro$temperature == 20,]

sing5<-function(min_L,max_L,min_K,max_K,min_t0,max_t0,x,to_rm)
{

  on_off="on"

  while(on_off=="on"){

    library(sampling)

    x2<-srswor(nrow(x)-to_rm,nrow(x))
    x2<-getdata(x,x2)


    Linf_ini=runif(1,min=min_L,max=max_L)
    K_ini=runif(1,min=min_K,max=max_K)
    t0_ini=runif(1,min=min_t0,max=max_t0)

    texto<-try(nls(length~Linf*(1-exp(-K*(age-t0))),data=x2,
    start=list(Linf=Linf_ini,K=K_ini,t0=t0_ini))
    ,silent=T)

    if(!strsplit(as.character(texto)," ")[[1]][1] %in% "Error"){
        on_off="off"
    }


  }

return(list(Linf_ini,K_ini,t0_ini,x2))
}

###Nevertheless, for consistency we adjust the SSfpl using Mario's approach with offset and David's code for starting values
fit_logistic4p_fixedA <- function(data, lengthAtBirth = NULL, max_iters = 1000, min_factor = 1e-6) {
    library(stats)
    
    # Compute or set A_fixed
    if (is.null(lengthAtBirth)) {
        A_fixed <- min(data$length)
    } else {
        A_fixed <- lengthAtBirth
    }
        formula <- as.formula(
        paste0("length ~ (", A_fixed, " - offset) + (B - (", A_fixed, " - offset)) / (1 + exp((xmid - age) / scal))")
    )
    start_fun <- function(data) {
        xy <- sortedXyData("age", "length", data)
        if (nrow(xy) < 3) stop("Too few distinct input values to fit the model")
        B <- max(xy$y)  # Upper asymptote
        xmid <- mean(xy$x)  # Midpoint
        scal <- (max(xy$x) - min(xy$x)) / 4  # Steepness
        offset = 0.0
        list(B = B, xmid = xmid, scal = scal, offset = offset)  # Starting values
    }
    
    start_values <- start_fun(data)
    model <- tryCatch({
        nls(
            formula,
            data = data,
            start = start_values,
            control = nls.control(maxiter = max_iters, minFactor = min_factor)
        )
    }, error = function(e) {
        stop("Failed to fit Logistic4P_FixedA model. Reason: ", e$message)
    })
    
    print(summary(model))
	
	list(model = model, A_fixed = A_fixed)
}

plot_logistic4p_fixedA <- function(data, fit_results) {
    model <- fit_results$model
    A_fixed <- fit_results$A_fixed

    predicted <- predict(model, newdata = data)
    plot(
		data$age, data$length,
        pch = 19, col = "blue", xlab = "Age", ylab = "Length",
        main = "Logistic4P_FixedA Model Fit"
    )
    lines(data$age, predicted, col = "red", lwd = 2)
    legend("topleft", legend = c("Data Points", "Fitted Curve"), col = c("blue", "red"), pch = c(19, NA), lty = c(NA, 1), lwd = c(NA, 2))

    print(model)
}




##10ºC
newdata<-chiro10

newdata<-newdata[order(newdata$age),] ##

##to guess where to start
max(newdata$length)


###removing to_rm random number of data  points
####this does work eventually - obviously the curve differs
####substantially from the published one
lista<-sing5(min_L=2,max_L=15,min_K=0.001,max_K=5.00,min_t0=-10,
max_t0=0,newdata,0)

plot(newdata$age,newdata$length)

##lista[[4]]

newdata<-lista[[4]]

bv<-nls(length~Linf*(1-exp(-K*(age-t0))),data=newdata,
start=list(Linf= lista[[1]],K=lista[[2]],t0=lista[[3]]))
newdata

# summary(bv)


##this result is clearly wrong, but for some reason fits well to the straight line
#VB is the wrong model despite AIC because K<0
# Parameters:
      # Estimate Std. Error t value Pr(>|t|)    
# Linf -20.65521   10.13466  -2.038 0.045269 *  
# K     -0.00882    0.00371  -2.377 0.020134 *  
# t0    -2.26969    0.55374  -4.099 0.000109 ***

##plot(newdata$age,predict(bv),type="n")

plot(newdata$age,newdata$length,type="n",xlim=c(0,max(newdata$age)+1),
ylim=c(0,max(predict(bv))))
points(newdata$age,newdata$length,col="black",lwd=6,pch=19)
lines(newdata$age,predict(bv),col="red",lwd=6,pch=19)

linear<-nls(length~a+b*age,data=newdata,start=list(a=-1.051,b=4.638))

summary(linear)

Parameters:
  Estimate Std. Error t value Pr(>|t|)    
a 0.273667   0.051128   5.353 9.85e-07 ***
b 0.212335   0.003105  68.394  < 2e-16 ***

linearA=0.273667
k10=0.212335


summary(linear)$parameters[2,1]
[1] 0.2123348

plot(newdata$age,newdata$length,type="n",xlim=c(0,max(newdata$age)+1),
ylim=c(0,max(predict(linear))))
points(newdata$age,newdata$length,col="black",lwd=6,pch=19)
lines(newdata$age,predict(linear),col="red",lwd=6,pch=19)

expo<-nls(length~exp(k*age + a) + b,data=newdata,start=list(k=0.1,a=0.1,b=0.1))

# summary(expo)


sslogis1<-nls(length~SSlogis(age,Asym,xmid,scal),data=newdata,control = nls.control(maxiter = 1000))

# summary(sslogis1)

Parameters:
     Estimate Std. Error t value Pr(>|t|)    
Asym   8.2788     0.4526   18.29   <2e-16 ***
xmid  18.3512     1.0713   17.13   <2e-16 ***
scal   8.5097     0.4803   17.72   <2e-16 ***

1/8.5097
[1] 0.117513


# plot(newdata$age,newdata$length,type="n",xlim=c(0,max(newdata$age)+1),
# ylim=c(0,max(predict(sslogis1))))
# points(newdata$age,newdata$length,col="black",lwd=6,pch=19)
# lines(newdata$age,predict(sslogis1),col="red",lwd=6,pch=19)


##cannot find a good solution - too many parameters for a clear linear relationship
ssfpl1<-nls(length~SSfpl(age,a,b,xmid,scal),data=newdata,control = nls.control(maxiter = 1000))


AIC(linear,expo,sslogis1)


cor(predict(expo),newdata$length)
cor(predict(linear),newdata$length)
# cor(predict(bv),newdata$length)
cor(predict(sslogis1),newdata$length)

lines(newdata$age,predict(sslogis1),col="purple",lwd=6,pch=19)

# lista<-sing5(min_L=2,max_L=15,min_K=0.001,max_K=5.00,min_t0=-10,
# max_t0=0,noveldata,0)


summary(expo)





##15ºC
newdata<-chiro15

newdata<-newdata[order(newdata$age),] ##

##to guess where to start
max(newdata$length)

###removing to_rm random number of data  points
####this does work eventually - obviously the curve differs
####substantially from the published one
lista<-sing5(min_L=2,max_L=15,min_K=0.001,max_K=5.00,min_t0=-10,
max_t0=0,newdata,0)

plot(newdata$age,newdata$length)

##lista[[4]]

newdata<-lista[[4]]

bv<-nls(length~Linf*(1-exp(-K*(age-t0))),data=newdata,start=list(Linf= lista[[1]]
,K=lista[[2]],t0=lista[[3]]))
newdata
# summary(bv)


##
# Parameters:
     # Estimate Std. Error t value Pr(>|t|)    
# Linf 9.122084   0.458589  19.892  < 2e-16 ***
# K    0.094060   0.008236  11.420 1.53e-15 ***
# t0   0.420305   0.094514   4.447 4.86e-05 ***

##plot(newdata$age,predict(bv),type="n")

plot(newdata$age,newdata$length,type="n",xlim=c(0,max(newdata$age)+1),
ylim=c(0,max(predict(bv))))
points(newdata$age,newdata$length,col="black",lwd=6,pch=19)
lines(newdata$age,predict(bv),col="red",lwd=6,pch=19)


linear<-nls(length~a+b*age,data=newdata,start=list(a=-5,b=1))

summary(linear)

Parameters:
  Estimate Std. Error t value Pr(>|t|)    
a 0.273667   0.051128   5.353 9.85e-07 ***
b 0.212335   0.003105  68.394  < 2e-16 ***

summary(linear)$parameters[2,1]
[1] 0.4835971


AIC(sslogis1,bv,linear)

sslogis1<-nls(length~SSlogis(age,Asym,xmid,scal),data=newdata,control = nls.control(maxiter = 1000))

# summary(sslogis1)

plot(newdata$age,newdata$length,type="n",xlim=c(0,max(newdata$age)+1),
ylim=c(0,max(predict(sslogis1))))
points(newdata$age,newdata$length,col="black",lwd=6,pch=19)
lines(newdata$age,predict(sslogis1),col="red",lwd=6,pch=19)

linear<-nls(length~a+b*age,data=newdata,start=list(a=-5,b=1))

AIC(sslogis1,bv,linear)


###Perfect match for a logistic of 4 parameters...

ssfpl1<-nls(length~SSfpl(age,a,b,xmid,scal),data=newdata,control = nls.control(maxiter = 1000))

summary(ssfpl1)
     Estimate Std. Error t value Pr(>|t|)    
a     -1.7224     0.5403  -3.188   0.0025 ** 
b      6.7109     0.1434  46.786  < 2e-16 ***
xmid   4.0316     0.3783  10.656 2.34e-14 ***
scal   3.2964     0.2767  11.915 4.39e-16 ***


plot(newdata$age,newdata$length,type="n",xlim=c(0,max(newdata$age)+1),
ylim=c(0,max(predict(ssfpl1))))
points(newdata$age,newdata$length,col="black",lwd=6,pch=19)
lines(newdata$age,predict(ssfpl1),col="red",lwd=6,pch=19)


fit_results <- fit_logistic4p_fixedA(newdata,lengthAtBirth=lengthAtBirth)
plot_logistic4p_fixedA(newdata, fit_results)


##Result from a perfect fit: we have the offset at 2.282, thus it is necessary
Nonlinear regression model
  model: length ~ (	 - offset) + (B - (0.56 - offset))/(1 + exp((xmid -     age)/scal))
   data: data
     B   xmid   scal offset 
 6.711  4.032  3.296  2.282 
 residual sum-of-squares: 0.8169

Number of iterations to convergence: 5 
Achieved convergence tolerance: 6.998e-06

summary(fit_results$model)

Parameters:
       Estimate Std. Error t value Pr(>|t|)    
B        6.7109     0.1434  46.786  < 2e-16 ***
xmid     4.0316     0.3783  10.656 2.34e-14 ***
scal     3.2964     0.2767  11.915 4.39e-16 ***
offset   2.2824     0.5403   4.225 0.000104 ***

k15=1/3.2964


AIC(sslogis1,ssfpl1,bv,linear,fit_results$model)

                  df       AIC
sslogis1           4 -37.32519
ssfpl1             5 -60.73721
bv                 4 -28.91474
linear             3  43.87606
fit_results$model  5 -60.73721  ##the fit is identical as the normal 4pl, indicating that probably there is no effect of the offset


plot(newdata$age,newdata$length,type="n",xlim=c(-7,max(newdata$age)+1),
ylim=c(-3,max(predict(bv))+1))
points(newdata$age,newdata$length,col="black",lwd=6,pch=19)
lines(c(-7:0,newdata$age),predict(bv,newdata=data.frame(age=c(-7:0,newdata$age))),
col="red",lwd=6,pch=19)




plot(newdata$age,newdata$length,type="n",xlim=c(0,max(newdata$age)+1),
ylim=c(0,max(predict(ssfpl1))))
points(newdata$age,newdata$length,col="black",lwd=6,pch=19)
lines(newdata$age,predict(ssfpl1),col="red",lwd=6,pch=19)

k = 1/coefficients(ssfpl1)[4]
k

   scal 
0.30336 

###This shows that timeStep works well for k even for the 4pl ##
################################################################
################################################################
newdata$age2<-newdata$age*(1/0.1)
ssfpl1.2<-nls(length~SSfpl(age2,a,b,xmid,scal),data=newdata,control = nls.control(maxiter = 1000))
summary(ssfpl1.2)

k2 = 1/coefficients(ssfpl1.2)[4]
k2

plot(newdata$age2,newdata$length,type="n",xlim=c(-7,max(newdata$age2)+1),
ylim=c(-3,max(predict(ssfpl1.2))+1))
points(newdata$age2,newdata$length,col="black",lwd=6,pch=19)
lines(c(-7:0,newdata$age2),predict(ssfpl1.2,newdata=data.frame(age2=c(-7:0,newdata$age2))),
col="red",lwd=6,pch=19)

aFpl=coefficients(ssfpl1.2)[1]
bFpl=coefficients(ssfpl1.2)[2]
xmidFpl=coefficients(ssfpl1.2)[3]

##4-parameter logistic by hand
# A+(B-A)/(1+exp((xmid-input)/scal))
eggLengthAtBirth = aFpl + ((bFpl - aFpl)/(1 + exp((xmidFpl-0)*k2)))
eggLengthAtBirth

points(0,eggLengthAtBirth,col="purple",cex=2,pch=17)
abline(h=aFpl,lty=2,lwd=3)
abline(h=bFpl,lty=2,lwd=3)

abline(v=xmidFpl,lty=2,lwd=3)
################################################################
################################################################

##eggLengthAtBirth
eggLengthAtBirth=predict(ssfpl1,newdata=data.frame(age=0))
eggLengthAtBirth
0.1953608

##eggLengthAtBirth is almost identical regardless of using the additional offset model or not, what differs must be aLogis?
eggLengthAtBirth=predict(fit_results$model,newdata=data.frame(age=0))
eggLengthAtBirth
[1] 0.1953646


# aFpl=coefficients(ssfpl1)[1]
# bFpl=coefficients(ssfpl1)[2]
# xmidFpl=coefficients(ssfpl1)[3]


# aLogis=aFpl
# aLogis



# ##4-parameter logistic by hand
# # A+(B-A)/(1+exp((xmid-input)/scal))
# eggLengthAtBirth = aFpl + ((bFpl - aFpl)/(1 + exp((xmidFpl-0)*k)))
# eggLengthAtBirth

##eggDryMass
coefficientForMassA = 0.002
scaleForMassB = 2.254

eggDryMass = coefficientForMassA*eggLengthAtBirth^scaleForMassB
eggDryMass
[1] 5.041928e-05


##How many eggs in one adult - 500 minimum must fit
lengthAtMaturation = predict(fit_results$model,data=data.frame(age=max(newdata$age)))
lengthAtMaturation

dryMassAtMaturation = coefficientForMassA*lengthAtMaturation^scaleForMassB

dryMassAtMaturation/eggDryMass
##Yesss,too many eggs perhaps, we could point at larger eggs?
2497.927

conversionToWetMass = 9.193

femaleWetMass=dryMassAtMaturation*conversionToWetMass

femaleWetMass
1.15775


points(0,eggLengthAtBirth,col="purple",cex=2,pch=17)




plot(newdata$age,newdata$length,type="n",xlim=c(-7,max(newdata$age)+1),
ylim=c(-3,max(predict(ssfpl1))+1))
points(newdata$age,newdata$length,col="black",lwd=6,pch=19)
lines(c(-7:0,newdata$age),predict(ssfpl1,newdata=data.frame(age=c(-7:0,newdata$age))),
col="red",lwd=6,pch=19)
points(0,eggLengthAtBirth,col="purple",cex=2,pch=17)
abline(h=aFpl,lty=2,lwd=3)
abline(h=bFpl,lty=2,lwd=3)
abline(v=xmidFpl,lty=2,lwd=3)

plot(newdata$age,newdata$length,type="n",xlim=c(-7,max(newdata$age)+1),
ylim=c(-3,max(predict(sslogis1))+1))
points(newdata$age,newdata$length,col="black",lwd=6,pch=19)
lines(c(-7:0,newdata$age),predict(sslogis1,newdata=data.frame(age=c(-7:0,newdata$age))),
col="red",lwd=6,pch=19)
points(0,eggLengthAtBirth,col="purple",cex=2,pch=17)
abline(h=aFpl,lty=2,lwd=3)
abline(h=bFpl,lty=2,lwd=3)
abline(v=xmidFpl,lty=2,lwd=3)


plot(newdata$age,newdata$length,type="n",xlim=c(-7,max(newdata$age)+1),
ylim=c(-3,max(predict(bv))+1))
points(newdata$age,newdata$length,col="black",lwd=6,pch=19)
lines(c(-7:0,newdata$age),predict(bv,newdata=data.frame(age=c(-7:0,newdata$age))),
col="red",lwd=6,pch=19)
points(0,eggLengthAtBirth,col="purple",cex=2,pch=17)
abline(h=aFpl,lty=2,lwd=3)
abline(h=bFpl,lty=2,lwd=3)
abline(v=xmidFpl,lty=2,lwd=3)

plot(newdata$age,newdata$length,type="n",xlim=c(-7,max(newdata$age)+1),
ylim=c(-3,max(predict(bv))+1))
points(newdata$age,newdata$length,col="black",lwd=6,pch=19)
lines(c(-7:0,newdata$age),predict(bv,newdata=data.frame(age=c(-7:0,newdata$age))),
col="red",lwd=6,pch=19)
points(0,eggLengthAtBirth,col="purple",cex=2,pch=17)
abline(h=aFpl,lty=2,lwd=3)
abline(h=bFpl,lty=2,lwd=3)
abline(h=0,lty=2,lwd=3,col="red")
abline(v=0,lty=2,lwd=3,col="red")



###Potential solution to avoid 4pl - add the lengthAtBirth as additional data point and switch to 3pl
addPoints<-aFpl + ((bFpl - aFpl)/(1 + exp((xmidFpl-seq(0,0.85,length.out=10))*k)))

newdata<-data.frame(age=c(seq(0,0.85,length.out=10),newdata$age),length=c(addPoints,newdata$length))

newdata<-newdata[order(newdata$age),] ##

##to guess where to start
max(newdata$length)

###removing to_rm random number of data  points
####this does work eventually - obviously the curve differs
####substantially from the published one
lista<-sing5(min_L=2,max_L=15,min_K=0.001,max_K=5.00,min_t0=-10,
max_t0=0,newdata,0)

plot(newdata$age,newdata$length)

##lista[[4]]

newdata<-lista[[4]]

bv<-nls(length~Linf*(1-exp(-K*(age-t0))),data=newdata,start=list(Linf= lista[[1]]
,K=lista[[2]],t0=lista[[3]]))
newdata
# summary(bv)


##
# Parameters:
     # Estimate Std. Error t value Pr(>|t|)    
# Linf 9.122084   0.458589  19.892  < 2e-16 ***
# K    0.094060   0.008236  11.420 1.53e-15 ***
# t0   0.420305   0.094514   4.447 4.86e-05 ***

##plot(newdata$age,predict(bv),type="n")

plot(newdata$age,newdata$length,type="n",xlim=c(0,max(newdata$age)+1),
ylim=c(0,max(predict(bv))))
points(newdata$age,newdata$length,col="black",lwd=6,pch=19)
lines(newdata$age,predict(bv),col="red",lwd=6,pch=19)


linear<-nls(length~a+b*age,data=newdata,start=list(a=-5,b=1))

AIC(sslogis1,bv,linear)

sslogis1<-nls(length~SSlogis(age,Asym,xmid,scal),data=newdata,control = nls.control(maxiter = 1000))

# summary(sslogis1)

plot(newdata$age,newdata$length,type="n",xlim=c(0,max(newdata$age)+1),
ylim=c(0,max(predict(sslogis1))))
points(newdata$age,newdata$length,col="black",lwd=6,pch=19)
lines(newdata$age,predict(sslogis1),col="red",lwd=6,pch=19)

linear<-nls(length~a+b*age,data=newdata,start=list(a=-5,b=1))

AIC(sslogis1,bv,linear)

ssfpl1<-nls(length~SSfpl(age,a,b,xmid,scal),data=newdata,control = nls.control(maxiter = 1000))

summary(ssfpl1)
 
AIC(sslogis1,ssfpl1,bv,linear)


plot(newdata$age,newdata$length,type="n",xlim=c(-7,max(newdata$age)+1),
ylim=c(-3,max(predict(ssfpl1))+1))
points(newdata$age,newdata$length,col="black",lwd=6,pch=19)
lines(c(-7:0,newdata$age),predict(ssfpl1,newdata=data.frame(age=c(-7:0,newdata$age))),
col="red",lwd=6,pch=19)


plot(newdata$age,newdata$length,type="n",xlim=c(-7,max(newdata$age)+1),
ylim=c(-3,max(predict(sslogis1))+1))
points(newdata$age,newdata$length,col="black",lwd=6,pch=19)
lines(c(-7:0,newdata$age),predict(sslogis1,newdata=data.frame(age=c(-7:0,newdata$age))),
col="red",lwd=6,pch=19)

plot(newdata$age,newdata$length,type="n",xlim=c(-7,max(newdata$age)+1),
ylim=c(-3,max(predict(bv))+1))
points(newdata$age,newdata$length,col="black",lwd=6,pch=19)
lines(c(-7:0,newdata$age),predict(bv,newdata=data.frame(age=c(-7:0,newdata$age))),
col="red",lwd=6,pch=19)


plot(newdata$age,newdata$length,type="n",xlim=c(-7,max(newdata$age)+1),
ylim=c(-3,max(predict(bv))+1))
points(newdata$age,newdata$length,col="black",lwd=6,pch=19)
lines(c(-7:0,newdata$age),predict(bv,newdata=data.frame(age=c(-7:0,newdata$age))),
col="red",lwd=6,pch=19)
points(0,eggLengthAtBirth,col="purple",cex=2,pch=17)
abline(h=aFpl,lty=2,lwd=3)
abline(h=bFpl,lty=2,lwd=3)
abline(h=0,lty=2,lwd=3,col="red")
abline(v=0,lty=2,lwd=3,col="red")






##20ºC
newdata<-chiro20

newdata<-newdata[order(newdata$age),] ##

##to guess where to start
max(newdata$length)


###removing to_rm random number of data  points
####this does work eventually - obviously the curve differs
####substantially from the published one
lista<-sing5(min_L=2,max_L=15,min_K=0.001,max_K=5.00,min_t0=-10,
max_t0=0,newdata,0)

plot(newdata$age,newdata$length)

##lista[[4]]

newdata<-lista[[4]]

bv<-nls(length~Linf*(1-exp(-K*(age-t0))),data=newdata,start=list(Linf= lista[[1]]
,K=lista[[2]],t0=lista[[3]]))
newdata
# summary(bv)


##this result is clearly wrong, but for some reason fits well to the straight line
# Parameters:
     # Estimate Std. Error t value Pr(>|t|)    
# Linf 8.525075   0.602355  14.153  < 2e-16 ***
# K    0.081322   0.009149   8.889 8.64e-12 ***
# t0   0.448975   0.102333   4.387 6.09e-05 ***

##plot(newdata$age,predict(bv),type="n")

plot(newdata$age,newdata$length,type="n",xlim=c(0,max(newdata$age)+1),
ylim=c(0,max(predict(bv))))
points(newdata$age,newdata$length,col="black",lwd=6,pch=19)
lines(newdata$age,predict(bv),col="red",lwd=6,pch=19)


sslogis1<-nls(length~SSlogis(age,Asym,xmid,scal),data=newdata,control = nls.control(maxiter = 1000))

# summary(sslogis1)

# plot(newdata$age,newdata$length,type="n",xlim=c(0,max(newdata$age)+1),
# ylim=c(0,max(predict(sslogis1))))
# points(newdata$age,newdata$length,col="black",lwd=6,pch=19)

linear<-nls(length~a+b*age,data=newdata,start=list(a=-5,b=1))

summary(linear)

  Estimate Std. Error t value Pr(>|t|)    
a 0.301104   0.061570    4.89 1.08e-05 ***
b 0.434537   0.008465   51.33  < 2e-16 ***

summary(linear)$parameters[2,1]
[1] 0.4345366

ssfpl1<-nls(length~SSfpl(age,a,b,xmid,scal),data=newdata,control = nls.control(maxiter = 1000))

summary(ssfpl1)

Parameters:
     Estimate Std. Error t value Pr(>|t|)    
a     -2.4641     1.2879  -1.913   0.0617 .  
b      5.8771     0.3200  18.368  < 2e-16 ***
xmid   3.2997     0.9730   3.391   0.0014 ** 
scal   3.8721     0.6955   5.567 1.14e-06 ***

1/coefficients(ssfpl1)[4]
0.2582557 


AIC(sslogis1,ssfpl1,bv,linear)

plot(newdata$age,newdata$length,type="n",xlim=c(0,max(newdata$age)+1),
ylim=c(0,max(predict(sslogis1))))
points(newdata$age,newdata$length,col="black",lwd=6,pch=19)
lines(newdata$age,predict(ssfpl1),col="red",lwd=6,pch=19)


fit_results <- fit_logistic4p_fixedA(newdata,lengthAtBirth=lengthAtBirth)
plot_logistic4p_fixedA(newdata, fit_results)


##4pl with offset, fixing lengthAtBirth as A
Formula: length ~ (0.61 - offset) + (B - (0.61 - offset))/(1 + exp((xmid - 
    age)/scal))

Parameters:
       Estimate Std. Error t value Pr(>|t|)    
B        5.8771     0.3200  18.368  < 2e-16 ***
xmid     3.2997     0.9730   3.391   0.0014 ** 
scal     3.8721     0.6955   5.568 1.14e-06 ***
offset   3.0741     1.2879   2.387   0.0210 *  


k20=1/3.8721



ch<-read.table("Chironomidae_Cricotopus.txt",h=T)
names(ch)

timeStep = 0.1

t10<-ch[ch$temperature == 10,]

# age=(t15$days*(1/timeStep))

age10=t10$days*(1/timeStep)

plot(age10,t10$mm,pch=16,xlim=c(0,max(age10)),ylim=c(0,max(t10$mm)))

points(age10,t10$mm,col="blue",pch=16,cex=1.1)



t15<-ch[ch$temperature == 15,]

age15=t15$days*(1/timeStep)

points(age15,t15$mm,col="purple",pch=16,cex=1.1)


t20<-ch[ch$temperature == 20,]

age20=t20$days*(1/timeStep)

points(age20,t20$mm,pch=16,cex=1.1,col="red")




##To obtain TPCs from growth curves
temps=c(10,15,20)
devTimes=c(max(t10$days),max(t15$days),max(t20$days))

devRates=1/devTimes

##Come from different models
ks=c(k10,k15,k20)


pawar_2018_GAs <- function(x,theta,Tref,E=NULL)
{

BOLTZMANN = 8.62E-5
T0Kelvin = 273.15
	
	# # x=predTemps
	# # theta=GAresult
	# # E=NULL
	
	x = x + T0Kelvin

	# pawar_2018_GAs<-function(x)
	# {
		if(is.null(E)){
				result1 = theta[1]*exp(-theta[2] * 
				((1/(BOLTZMANN*(x))) - (1/(BOLTZMANN*(Tref+T0Kelvin))))) /
				(1 + (theta[2] / 
				(theta[3] - theta[2])) * exp((theta[3] / BOLTZMANN)* 
				((1/(theta[4]+T0Kelvin)) - (1/x))))
				# Topt<-theta[4]
		}else{
				result1 = theta[1]*exp(-E * 
				((1/(BOLTZMANN*(x))) - (1/(BOLTZMANN*(Tref+T0Kelvin))))) /
				(1 + (E / 
				(theta[2] - E)) * exp((theta[2] / BOLTZMANN)* 
				((1/(theta[3]+T0Kelvin)) - (1/x))))
				# Topt<-theta[3]
		}
	# return(result1)
	# }
	
	
	# result1<-pawar_2018_GAs(x)
	

return(result1)
}



nIniPop=1000
fractBest = 0.1
# nParms=6
nOffs = 1000
nGenerations = 200
mutRate=0.1
# genStartMutation = 0


GAs<-function(TPC,lower,upper,Tref,E=NULL) 
{

	 library(GA)
	# TPC<-forPost

	if(is.null(E)){
	
		fitnessL2 <- function(theta, x, y,Tref) -sum((y - pawar_2018_GAs(x,theta,Tref,E=NULL))^2)

		GA2 <- ga(type = "real-valued",seed=123,pmutation=mutRate, 
		Tref=Tref,
		fitness = fitnessL2,
		x = TPC$temperature, y = TPC$traitValue, lower = lower[1:4], 
		upper = upper[1:4],
		popSize = 500, crossover = gareal_blxCrossover, maxiter = 5000,
		run = 200,monitor=FALSE,keepBest=TRUE,
		names = c("traitAtTref", "E", "ED","Topt"),
		elitism = base::max(1, round(nOffs*fractBest)),parallel=FALSE
		# # parallel = parallel::detectCores()
		)  ##names = c("a", "b", "c")

	result<-summary(GA2)$solution[1,1:4]	
	
	}else{

		fitnessL2 <- function(theta, x, y,Tref) -sum((y - pawar_2018_GAs(x,theta,Tref,E=E))^2)

		GA2 <- ga(type = "real-valued",seed=123,pmutation=mutRate, 
		Tref=Tref,
		fitness = fitnessL2,
		x = TPC$temperature, y = TPC$traitValue, lower = lower[1:3], 
		upper = upper[1:3],
		popSize = 500, crossover = gareal_blxCrossover, maxiter = 5000,
		run = 200,monitor=FALSE,keepBest=TRUE,
		names = c("traitAtTref","ED","Topt"),
		elitism = base::max(1, round(nOffs*fractBest)),parallel=FALSE
		# parallel = parallel::detectCores()
		)  ##names = c("a", "b", "c")

	
	result<-summary(GA2)$solution[1,1:3]	
	
	}

	print(summary(GA2))
	
	
return(result)
}


truncateByTraitAtTopt<-function(x,GAresult,Tref,E=NULL)
{


	if(is.null(E)){
	  Topt=GAresult[4]
	}else{
	  Topt=GAresult[3]	
	}
	
	traitAtTopt<-pawar_2018_GAs(Topt,GAresult,Tref,E=E)

	propOftraitAtTopt=1e-01
	nextToZeroTrait = propOftraitAtTopt*traitAtTopt
	nextToZeroTrait

	temp=x
	toCut<-data.frame(temp,trait=pawar_2018_GAs(temp,GAresult,Tref,E=E))

	thisCut<-toCut[toCut$trait>nextToZeroTrait,]

	left<-thisCut[thisCut$temp<Topt,]
	predTmin<-left$temp[which(left$trait==min(left$trait))]

	right<-thisCut[thisCut$temp>Topt,]
	predTmax<-right$temp[which(right$trait==min(right$trait))]

	result1<-ifelse(x<predTmin | x>predTmax ,0,toCut$trait) 

return(list(result1,predTmin,predTmax))
}



getGAresult<-function(traits,Tref,ToptIni,EIni,EDIni,traitAtTempRefIni,E)
{

	if(is.null(E)){
		lower =	c(min(traitAtTempRefIni),min(EIni),min(EDIni),min(ToptIni))
		upper = c(max(traitAtTempRefIni),max(EIni),max(EDIni),max(ToptIni))
		GAresult<-GAs(TPC=traits,lower,upper,Tref,E)
	}else{
		lower =	c(min(traitAtTempRefIni),min(EDIni),min(ToptIni))
		upper = c(max(traitAtTempRefIni),max(EDIni),max(ToptIni))
		GAresult<-GAs(TPC=traits,lower,upper,Tref,E)
	}

return(GAresult)
}




###devRates
Tref=10  ##which corresponds to minimum tempFromLab

traits<-data.frame(temperature=temps,traitValue=devRates) 

	ToptIni = seq(min(temps),max(temps),length.out=10) 
	EIni = seq(0.1,2.5,length.out=10)
	EDIni = seq(0.5,20,length.out=10)
	traitAtTempRefIni = seq(0,max(traits$traitValue,na.rm=T),length.out=10)

predTemps=seq(-30,60,length.out=100)


GAresult<-getGAresult(traits,Tref,ToptIni,EIni,EDIni,traitAtTempRefIni,E=NULL)

# GA results: 
# Iterations             = 300 
# Fitness function value = -4.256517e-11 
# Solution = 
     # traitAtTref        E       ED    Topt
# [1,]  0.02951888 1.061567 8.056491 18.5731

traitsAndTempLimits<-truncateByTraitAtTopt(predTemps,GAresult,Tref,E=NULL)

predTraits<-traitsAndTempLimits[[1]]

predTmin<-traitsAndTempLimits[[2]]
predTmax<-traitsAndTempLimits[[3]]

predTmin
predTmax

dev_rates_params<-GAresult



GAresult_fixedE<-getGAresult(traits,Tref,ToptIni,EIni,EDIni,traitAtTempRefIni,E=0.3)

traitsAndTempLimits2=truncateByTraitAtTopt(predTemps,GAresult_fixedE,Tref,E=0.3)

predTraits2<-traitsAndTempLimits2[[1]]

predTmin2<-traitsAndTempLimits2[[2]]
predTmax2<-traitsAndTempLimits2[[3]]

predTmin2
predTmax2

GAresult_fixedE<-getGAresult(traits,Tref,ToptIni,EIni,EDIni,traitAtTempRefIni,E=0.8)

traitsAndTempLimits3=truncateByTraitAtTopt(predTemps,GAresult_fixedE,Tref,E=0.8)

predTraits3<-traitsAndTempLimits3[[1]]

predTmin3<-traitsAndTempLimits2[[2]]
predTmax3<-traitsAndTempLimits2[[3]]

predTmin3
predTmax3




plot(predTemps,predTraits,type="l",col="red",
lwd=3,xlim=c(-30,60),ylim=c(0,max(predTraits)))

lines(predTemps,predTraits2,col="blue",
lwd=3,lty=2)

lines(predTemps,predTraits3,col="green",
lwd=3,lty=2)


points(traits$temperature,traits$traitValue,col="red",pch=16,cex=2)



##ks
Tref=10  ##which corresponds to minimum tempFromLab

traits<-data.frame(temperature=temps,traitValue=ks) 


##Increase range to force convergence
	ToptIni = seq(min(temps),max(temps)+10,length.out=10) 
	EIni = seq(0.1,2.5,length.out=10)
	EDIni = seq(0.5,20+10,length.out=10)
	traitAtTempRefIni = seq(0,max(traits$traitValue,na.rm=T),length.out=10)


predTemps=seq(-30,60,length.out=100)

GAresult<-getGAresult(traits,Tref,ToptIni,EIni,EDIni,traitAtTempRefIni,E=NULL)

traitsAndTempLimits<-truncateByTraitAtTopt(predTemps,GAresult,Tref,E=NULL)

predTraits<-traitsAndTempLimits[[1]]

predTmin<-traitsAndTempLimits[[2]]
predTmax<-traitsAndTempLimits[[3]]

predTmin
predTmax


ks_params<-GAresult



GAresult_fixedE<-getGAresult(traits,Tref,ToptIni,EIni,EDIni,traitAtTempRefIni,E=0.3)

traitsAndTempLimits2=truncateByTraitAtTopt(predTemps,GAresult_fixedE,Tref,E=0.3)

predTraits2<-traitsAndTempLimits2[[1]]

predTmin2<-traitsAndTempLimits2[[2]]
predTmax2<-traitsAndTempLimits2[[3]]

predTmin2
predTmax2

GAresult_fixedE<-getGAresult(traits,Tref,ToptIni,EIni,EDIni,traitAtTempRefIni,E=0.8)

traitsAndTempLimits3=truncateByTraitAtTopt(predTemps,GAresult_fixedE,Tref,E=0.8)

predTraits3<-traitsAndTempLimits3[[1]]

predTmin3<-traitsAndTempLimits2[[2]]
predTmax3<-traitsAndTempLimits2[[3]]

predTmin3
predTmax3


plot(predTemps,predTraits,type="l",col="red",
lwd=3,xlim=c(-30,60),ylim=c(0,max(predTraits)))

lines(predTemps,predTraits2,col="blue",
lwd=3,lty=2)

lines(predTemps,predTraits3,col="green",
lwd=3,lty=2)


points(traits$temperature,traits$traitValue,col="red",pch=16,cex=2)



pawar_2018 <- function(x,theta,Tref)
{

BOLTZMANN = 8.62E-5
T0Kelvin = 273.15
	
	x = x + T0Kelvin

	result1 = theta$traitAtTref*exp(-theta$E * 
	((1/(BOLTZMANN*(x))) - (1/(BOLTZMANN*(Tref+T0Kelvin))))) /
	(1 + (theta$E / 
	(theta$ED - theta$E)) * exp((theta$ED / BOLTZMANN)* 
	((1/(theta$Topt+T0Kelvin)) - (1/x))))

return(result1)
}

growthCurve<-function(Lmax = NULL,k = NULL,t = NULL, tmax = NULL, eggDryMass = NULL, 
aLogis = NULL, growthCurve.type = NULL, offset = NULL)
{

	# lengthAtBirth = predict(model,newdata=data.frame(age=0))
	
	# if(growthCurve.type == "VonBertalanffy"){

		# Linf = (Lmax*exp(k*tmax)-lengthAtBirth)/(exp(k*tmax)-1)		       
			
	    # t0 = log(1-(lengthAtBirth/Linf))/k
		
		# L = Linf*(1-exp(-k*(t-t0)))
	
	# }

	# if(growthCurve.type == "Logistic"){
		
		# Linf = (Lmax*lengthAtBirth*(exp(k*tmax)-1))/(lengthAtBirth*exp(k*tmax)-Lmax)	
		
		# xmid = log((Linf-lengthAtBirth)/lengthAtBirth)*(1/k)
		
		# L = Linf/(1+exp((xmid-t)*k))
		
		
	# }
	
	if(growthCurve.type == "linear"){
	   
		L = lengthAtBirth + k*t

	}

	if(growthCurve.type == "Logistic4P"){
	   
		##Note 17/1/2025 "aLogis" is mininum length (approx lengthAtBirth) - offset" where offset is obtained by fitting the model. 
		##This procedure allows matching the model to the data while not making lengthAtBirth going off too much. 
		##The case of Pardosa.
		
		# Lmax = Lmax;k = k;t = age; tmax = devTime; eggDryMass = NULL; 
		# aLogis = NULL; growthCurve.type = "Logistic4P"; offset = offset
		
		# offset=5
		
		aLogis = lengthAtBirth - offset
		
		xmid = log((-lengthAtBirth + Lmax)*exp(k*tmax)/(lengthAtBirth*exp(k*tmax) - 
		Lmax - aLogis*exp(k*tmax) + aLogis))/k
				
		Linf = lengthAtBirth*exp(k*xmid) + lengthAtBirth - aLogis*exp(k*xmid)
		
		# print(c(xmid,Linf))

	    L = aLogis+(Linf-aLogis)/(1+exp((xmid-t)*k))
	
	}
	
	# if(growthCurve.type == "Exponential"){
	
		# aExpo = log((Lmax-lengthAtBirth)/(exp(k*tmax)-1))

		# bExpo = lengthAtBirth-exp(aExpo)
	
		# L = exp(k*t + aExpo) + bExpo  
	
	# }

return(L)
}



TSRpoints = data.frame(temps=c(10,15,20),lengths=c(max(t10$mm),max(t15$mm),max(t20$mm)))
TSRpoints

  temps lengths
1    10    6.66
2    15    6.29
3    20    5.00


interpol2<-function(x,minx,maxx,miny,maxy)  ##interpolation for values
{

	y=miny+((x-minx)*((maxy-miny)/(maxx-minx)))

return(y)
}

calculateTSR<-function(temp)
{

	if(temp<TSRpoints$temps[2]){
	   return(interpol2(temp,TSRpoints$temps[1],TSRpoints$temps[2],TSRpoints$lengths[1],TSRpoints$lengths[2]))
	}else{
	   return(interpol2(temp,TSRpoints$temps[2],TSRpoints$temps[3],TSRpoints$lengths[2],TSRpoints$lengths[3]))
	}

}

offset15=2.2824
offset20=3.0741

calculateOffset<-function(temp)
{
  return(interpol2(temp,15,20,offset15,offset20))
}




# tempCurves = seq(10,20,length.out=3)
tempCurves = seq(20,22,length.out=20)

maxDevTime = max(1/pawar_2018(tempCurves,as.data.frame(t(dev_rates_params)),Tref))
maxAge = seq(0,maxDevTime,length.out=100)

kMin = pawar_2018(min(tempCurves),as.data.frame(t(ks_params)),Tref)

##For plotting purposes: calculate maximum body length at minimum temperature whether from TSR or from linear growth
maxLmax = max(calculateTSR(min(tempCurves)),lengthAtBirth+kMin*maxDevTime)

plot(maxAge,seq(0,maxLmax,length.out=length(maxAge)),type="n")


temperatureThresholdForGrowthModel = 14

count=1
for(i in tempCurves){
# i=0

	devTime = 1/pawar_2018(i,as.data.frame(t(dev_rates_params)),Tref)
	
	age = seq(0,devTime,length.out=100)
	
	Lmax = calculateTSR(i)

	if(i <= temperatureThresholdForGrowthModel){ ##linear
	   
	   k = pawar_2018(i,as.data.frame(t(ks_params)),Tref)
	   
       L<-growthCurve(Lmax = Lmax,k = k,t = age, tmax = devTime, eggDryMass = NULL, 
	   aLogis = NULL, growthCurve.type = "linear", offset = NULL)
	   
	}else{ ##4pl
	
       k = pawar_2018(i,as.data.frame(t(ks_params)),Tref)

		offset=calculateOffset(i)
		L<-growthCurve(Lmax = Lmax,k = k,t = age, tmax = devTime, eggDryMass = NULL, 
		aLogis = NULL, growthCurve.type = "Logistic4P", offset = offset)

	}

print(c("temperature: ",i))
print(L)

# if(count==1) col="blue"
# if(count==2) col="green"
# if(count==3) col="red"
# lines(age,L,col=col,lwd=3)   ##round(runif(1,1,50))

lines(age,L,col=round(runif(1,1,50)),lwd=3) 

count=count+1
}




