a <- scan("sim-5-10ms.acr", list(lag=0,auto=0))
lag <- a$lag[2:length(a$lag)];
auto <- a$auto[2:length(a$auto)];
l <- lm(log10(auto)~log10(lag))
postscript(landscape=FALSE,width=6,height=6)
plot(log10(lag), log10(auto), type="l")
abline(coef(l))
summary(l)
