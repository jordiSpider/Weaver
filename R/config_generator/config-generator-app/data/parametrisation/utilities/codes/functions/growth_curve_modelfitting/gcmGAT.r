#' @title gcmGAT function: Growth Curve Model Fitting with Temperature using GA-enhanced nlsLM (Raw Data)
#'
#' @description
#' The gcmGAT function fits a suite of candidate growth models to raw developmental data, optionally stratified by temperature.
#' It employs a hybrid optimization scheme using Genetic Algorithms (GA) for a global search of starting parameter values
#' and non-linear least squares minimization (nlsLM) for local refinement. The methodology preserves the original measurement scales,
#' ensuring that all parameters retain their biological interpretability.
#'
#' The candidate models implemented include:
#' \itemize{
#'   \item \strong{Exponential}:  
#'         \deqn{length = \exp(a_{\text{Expo}}) \times \exp(k \times age) + b_{\text{Expo}},}
#'         suitable for early growth phases where an approximately exponential change is observed.
#'
#'   \item \strong{Von Bertalanffy}:  
#'         Two formulations are provided:
#'         \itemize{
#'           \item \emph{Standard 3-parameter VB model} (used when no reliable length at birth information is available):  
#'                \deqn{length = L_\infty \Bigl(1-\exp\bigl(-k\,(age-t_0)\bigr)\Bigr),}  
#'                where \eqn{L_\infty} is the asymptotic maximum length, \eqn{k} is the growth rate, and \eqn{t_0} is the theoretical 
#'                age at which length would be zero.
#'
#'           \item \emph{Deterministic 2-parameter approach} (used when a known length at birth (\eqn{L_0}) is provided or an observation 
#'                at \code{age = 0} is available):  
#'                The model is first fitted as  
#'                \deqn{length = L_0 + \Bigl(L_\infty-L_0\Bigr)\Bigl(1-\exp(-k\,age)\Bigr),}
#'                fixing \eqn{L_0} to the known value. Because numerous studies (e.g., von Bertalanffy, 1957; Pauly, 1981; Schnute, 1981) 
#'                have documented that when \eqn{L_0} is known the collinearity between \eqn{L_\infty} and \eqn{k} is very high—rendering the independent estimation 
#'                of \eqn{t_0} unstable—the parameter \eqn{t_0} is computed deterministically as  
#'                \deqn{t_0 = \frac{\ln\left(1-\frac{L_0}{L_\infty}\right)}{k}.}  
#'                The final deterministic VB model, used for prediction and plotting, is then constructed as  
#'                \deqn{length = L_\infty\Bigl(1-\exp\bigl(-k\,(age-t_0)\bigr)\Bigr).}
#'         }
#'
#'   \item \strong{Logistic}:  
#'         \deqn{length = \frac{Asym}{1+\exp\Bigl(\frac{xmid-age}{scal}\Bigr)},}
#'         a sigmoidal model that captures the rapid initial growth and subsequent saturation, where \eqn{Asym} is the asymptotic limit, 
#'         \eqn{xmid} is the age at the inflection point, and \eqn{scal} governs the slope around the inflection.
#'
#'   \item \strong{Logistic4P_New}:  
#'          We model the length–age relationship with a four-parameter logistic (4-PL)
#'           curve: L0 (size at hatching), Lmax (asymptotic size), xmid (age at the
#'           midpoint) and scal (slope control). Each parameter is given “breathing room”
#'           because small errors in any one of them distort different parts of the curve,
#'           yet each is bounded within biologically sensible limits: L0 may vary ±20 %
#'           around the smallest observed length, Lmax ±5 % around the largest, xmid only
#'           within the observed age range, and scal between 0.01 and the full age span. A
#'           genetic algorithm searches this hyper-cube to find starting values that
#'           minimise the sum of squares—giving the earliest ages five-fold weight so the
#'           curve starts correctly—and nlsLM then fine-tunes the fit. The resulting curve
#'           passes almost exactly through the first and last data points keeps realistic
#'           asymptotes
#'   \item \strong{Linear}:  
#'         \deqn{length = a + b \times age,}
#'         a simple model that may act as a baseline. Although linear models often lack the saturation characteristic of biological growth,
#'         additional penalization is applied when the data indicate saturation, thus discouraging selection of an unrealistic linear model.
#' }
#'
#' @details
#' **I. Mathematical, Statistical, and Ecological Literature on Growth Curves:**  
#' The growth of organisms has been modeled using a variety of formulations. The Von Bertalanffy model (von Bertalanffy, 1957) is a cornerstone 
#' in growth theory, widely used in fisheries and ecological studies. When the length at birth (\eqn{L_0}) is known, the high collinearity between 
#' \eqn{L_\infty} and \eqn{k} often makes the free estimation of \eqn{t_0} unstable (Pauly, 1981; Schnute, 1981).  
#' Logistic models (dating back to Verhulst, 1838) capture sigmoidal growth and are prevalent in ecological and population studies.  
#' Linear models, although simplistic, may serve as useful null models for comparison (Sibly & Hone, 2002).  
#' The Logistic4P_New model extends the logistic approach by providing additional flexibility to capture asymmetries observed in empirical data 
#' (Ritz & Streibig, 2005).
#'
#' **II. Package Literature and Computational Tools:**  
#' The implementation of gcmGAT leverages several R packages:
#' \itemize{
#'   \item The \code{minpack.lm} package (available on CRAN) is utilized for nonlinear least-squares minimization via the Levenberg–Marquardt algorithm 
#'         (Bates & Watts, 1988).
#'   \item The \code{GA} package (available on CRAN) implements Genetic Algorithms for global optimization (Goldberg, 1989; Whitley, 1994).
#'   \item The \code{MuMIn} package is used for multi-model inference and model selection criteria computation (Barton, 2016).
#'   \item Additional computational work is supported by standard R functions and packages for data manipulation and plotting.
#' }
#'
#' **III. Model Implementation and Workflow:**  
#' - Each candidate model uses a GA-driven global search to obtain good initial parameter estimates, which are then refined via nlsLM.
#' - For the Von Bertalanffy model, if deterministic conditions are met (i.e., a known length at birth is provided or an observation at age 0 is present),  
#'   a two-parameter version is fitted and then transformed into a deterministic VB model by calculating \eqn{t_0 = \ln(1-\frac{L_0}{L_\infty})/k}.  
#'   The final deterministic model is rebuilt as an \code{nls} object with fixed parameter bounds, allowing it to be directly used for prediction.
#' - The function returns a list of candidate models in a named list, enabling easy access (e.g., \code{fitted$all_models$VonBertalanffy$fit} for the flexible version or  
#'   \code{fitted$all_models$VonBertalanffy_deterministic$fit} for the deterministic VB model).
#' - When a temperature variable is provided, the function fits models for each unique temperature subset; if not provided, temperature-related fields  
#'   are omitted from the output summary.
#'
#' @param data A data frame containing raw developmental data.
#' @param x A character string specifying the name of the independent variable (e.g., "age").
#' @param y A character string specifying the name of the dependent variable (e.g., "length").
#' @param nlsMcontrol A list of control parameters for the \code{nlsLM} algorithm (e.g., \code{list(maxiter = 1024, ftol = 1e-12, ptol = 1e-12, gtol = 1e-12)}).
#' @param controlGA A list of control parameters for the GA algorithm (e.g., \code{list(popSize = 100, maxiter = 10000, run = 100, seed = 123)}).
#' @param force_model Optionally, force the use of one candidate model. Valid options are:
#'   \code{"Exponential"}, \code{"Von Bertalanffy"}, \code{"Logistic"}, \code{"Logistic4P_New"}, or \code{"Linear"}.  
#'   The default is \code{NULL}, which fits all candidate models.
#' @param temperature Either a numeric value to filter the data by a specific temperature or the string \code{"ALL"}
#' to process all unique temperature values present in the data.
#' @param model_choose A character string specifying the model selection strategy.  
#' If \code{"auto"}, the function automatically selects the best model based on RSS, AICc, or log-likelihood; otherwise, all candidates are returned.
#' @param lengthAtBirth (Optional) A numeric value indicating a known length at birth (or an observation at \code{age = 0}), used to fix \eqn{L_0} in the Von Bertalanffy model.
#' @param lower (Optional) Additional lower bounds for parameters (reserved for future use).
#'
#' @return A list containing:
#' \itemize{
#'   \item \code{best_model_name}: A character string with the name of the best-fitting candidate model.
#'   \item \code{best_model}: The fitted nlsLM model object corresponding to the best candidate.
#'   \item \code{all_models}: A named list of candidate model results, accessible by model name (e.g., \code{fitted$all_models$VonBertalanffy$fit} or \code{fitted$all_models$VonBertalanffy_deterministic$fit}).
#'   \item \code{model_plots}: A recorded plot object (using \code{recordPlot()}) showing a grid of observed versus predicted curves.
#'   \item \code{adjusted_models_table}: A data frame summarising model selection statistics (AIC, AICc, RSS, log-likelihood, degrees of freedom, and pairwise p-values).  
#'         If no temperature is provided, the Temperature column is omitted.
#'   \item \code{pairwise_comparisons_table}: A data frame containing pairwise likelihood ratio comparisons (p-values) among candidate models.
#'   \item \code{growth}: The extracted growth parameter from the best candidate model.
#'   \item \code{temperature}: The temperature value associated with the current subset of data (if applicable).
#' }
#'
#' @references
#' **Package Literature:**
#' \itemize{
#'   \item Bates, D. M. and Watts, D. G. (1988). _Nonlinear Regression Analysis and Its Applications_. Wiley.
#'   \item Goldberg, D. E. (1989). _Genetic Algorithms in Search, Optimization, and Machine Learning_. Addison-Wesley.
#'   \item Whitley, D. (1994). "A Genetic Algorithm Tutorial." _Statistics and Computing_, 4, 65–85.
#'   \item Ruckdeschel, P., Kohl, M., and Horbenko, C. (2006). "Robust Nonlinear Regression in R – the package robustbase." _R News_.
#'   \item Barton, K. (2016). _MuMIn: Multi-Model Inference_. R package version 1.42.1.
#'   \item The _minpack.lm_ package is used for nonlinear least-squares minimization via the Levenberg–Marquardt algorithm.
#'   \item The _GA_ package is used for global optimization via genetic algorithms.
#' }
#'
#' **Ecological, Mathematical and Statistical Literature on Growth Curves:**
#' \itemize{
#'   \item von Bertalanffy, L. (1957). "Quantitative laws in metabolism and growth." _The Quarterly Review of Biology_, 32(3), 217–231.
#'   \item Pauly, D. (1981). "On the interrelationships between natural mortality, growth parameters, and the effects of fishing." _ICES Journal of Marine Science_, 42, 35–44.
#'   \item Schnute, J. (1981). "A general theory of growth, development, and mortality in fishes." _Canadian Journal of Fisheries and Aquatic Sciences_, 38, 845–866.
#'   \item Sibly, R. M. & Hone, J. (2002). "How should we model animal growth?" _Oikos_, 99(2), 243–251.
#'   \item Ritz, C. & Streibig, J. C. (2005). "Bioassay Analysis using R." _Journal of Statistical Software_, 12(5), 1–22.
#'   \item Verhulst, P.-F. (1838). "Notice sur la loi que la population suit dans son accroissement." _Correspondance Mathématique et Physique_, 10, 113–121.
#' }
#'
#' @author
#' David P. Quevedo & Mario Carmona-Segovia
#'
#' @supervisor
#' Jordi Moya-Laraño
#'
#' @date
#' Last modified: 2025-07-04
#'
#' @examples
#' \dontrun{
#' # Example 1: Fit models for a specific temperature (e.g., 20 ºC)
#' data <- read.csv("development_data.csv")
#' results <- gcmGAT(data = data, x = "age", y = "length", temperature = 20,
#'                   model_choose = "auto", lengthAtBirth = 0.5)
#' replayPlot(results$model_plots)
#'
#' # Accessing candidate models:
#' vb_original <- results$all_models$VonBertalanffy$fit            # The 2-parameter flexible VB model used for fitting/plotting.
#' vb_det <- results$all_models$VonBertalanffy_deterministic$fit     # The deterministic VB model rebuilt for prediction/interpretation.
#' coef(vb_det)  # Should show coefficients "Linf", "k", and "t0".
#' }
#'
#' @export

gcmGAT <- function(
    data, 
    x, 
    y, 
    nlsMcontrol = list(maxiter = 1024, ftol = 1e-12, ptol = 1e-12, gtol = 1e-12),
    controlGA = list(popSize = 100, maxiter = 10000, run = 100, seed = 123),
    force_model = NULL, 
    model_choose = "auto",
    lengthAtBirth = NULL,
    lower = NULL,
    use_app = FALSE
    ) {


    fit_data <- function(data, nlsMcontrol, controlGA, force_model, model_choose, lengthAtBirth, lower) {

        # ===========================
        # Helper: Early penalty function (for weighting early-age data)
        early_penalty <- function(age, early_factor = 5) {
            threshold <- as.numeric(quantile(age, 0.1))
            ifelse(age < threshold, 1 + early_factor * (1 - age/threshold), 1)
        }
        
        # ===========================
        # Helper: Build Deterministic VB Model.
        # Given a fitted 2-parameter VB model, extract Linf and k, compute t0 deterministically, and
        # re-fit an nls object using the conventional formula.
        build_deterministic_VB <- function(model_2p, data, L0_fixed = NULL, nlsLM_ctrl) {
            if (is.null(L0_fixed)) {
            if (any(data$age == 0)) {
                L0_fixed <- data$length[data$age == 0][1]
            } else {
                L0_fixed <- min(data$length)
            }
            }
            coefs <- coef(model_2p)
            Linf_est <- coefs[["Linf"]]
            k_est    <- coefs[["k"]]
            ratio <- 1 - (L0_fixed / Linf_est)
            t0_est <- if (ratio <= 0 || k_est <= 0) 0 else log(ratio) / k_est
            final_formula <- as.formula("length ~ Linf*(1 - exp(-k*(age - t0)))")
            
            final_model <- minpack.lm::nlsLM(
            formula = final_formula,
            data = data,
            start = list(Linf = Linf_est, k = k_est, t0 = t0_est),
            lower = c(Linf = Linf_est, k = k_est, t0 = t0_est),
            upper = c(Linf = Linf_est, k = k_est, t0 = t0_est),
            control = nlsLM_ctrl
            )
            final_coef <- coef(final_model)
            names(final_coef) <- c("Linf", "k", "t0")
            final_model$coefficients <- final_coef
            attr(final_model, "deterministic_formula") <- 
            paste0("length ~ ", format(Linf_est), "*(1 - exp(-", 
                    format(k_est), "*(age - ", format(t0_est), ")))")
            return(final_model)
        }

        # Helper: check if data has an observation at age = 0.
        has_age0 <- function(data) any(data$age == 0)
        length_at_age0 <- function(data) data$length[data$age == 0][1]



        nlsLM_ctrl <- minpack.lm::nls.lm.control(
            maxiter = nlsMcontrol$maxiter, 
            ftol = nlsMcontrol$ftol, 
            ptol = nlsMcontrol$ptol,
            gtol = nlsMcontrol$gtol
        )



        ############################################################################
        # Candidate models (using GA for initial search)
        ############################################################################
        candidate_models <- list(
            # 1) Exponential
            Exponential = function(data) {
            if (!is.null(lengthAtBirth)) {
                bExpo_lower <- 0.95 * lengthAtBirth
                bExpo_upper <- 1.05 * lengthAtBirth
            } else {
                bExpo_lower <- max(0, 0.8 * min(data$length))
                bExpo_upper <- 1.2 * min(data$length)
            }
            rss_fun <- function(par) {
                k     <- par[1]
                aExpo <- par[2]
                bExpo <- par[3]
                pred  <- exp(aExpo) * exp(k * data$age) + bExpo
                if (any(!is.finite(pred))) return(1e10)
                resid  <- data$length - pred
                weight <- early_penalty(data$age, early_factor = 5)
                sum((resid^2) * weight)
            }
            lower_bound <- c(k = 0, aExpo = -10, bExpo = bExpo_lower)
            upper_bound <- c(k = 1, aExpo = 10, bExpo = bExpo_upper)
            ga_out <- GA::ga(
                type    = "real-valued",
                fitness = function(par) -rss_fun(par),
                lower   = lower_bound,
                upper   = upper_bound,
                popSize = controlGA$popSize,
                maxiter = controlGA$maxiter,
                run     = controlGA$run,
                seed    = controlGA$seed,
                monitor = FALSE
                
            )
            init_par <- slot(ga_out, "solution")[1, ]
            names(init_par) <- c("k", "aExpo", "bExpo")
            form_exp <- as.formula("length ~ exp(aExpo)*exp(k*age) + bExpo")
            minpack.lm::nlsLM(form_exp,
                                data    = data,
                                start   = as.list(init_par),
                                lower   = lower_bound,
                                upper   = upper_bound,
                                control = nlsLM_ctrl)
            },
            
            # 2) Von Bertalanffy
            VonBertalanffy = function(data) {
            if (!is.null(lengthAtBirth) || has_age0(data)) {
                L0_fixed <- if (!is.null(lengthAtBirth)) lengthAtBirth else length_at_age0(data)
                rss_fun_2 <- function(par) {
                Linf <- par[1]; k <- par[2]
                pred <- L0_fixed + (Linf - L0_fixed)*(1 - exp(-k*data$age))
                if (any(!is.finite(pred))) return(1e10)
                sum((data$length - pred)^2 * early_penalty(data$age, early_factor=5))
                }
                lower2 <- c(Linf=0.9*max(data$length), k=1e-6)
                upper2 <- c(Linf=1.5*max(data$length), k=1)
                ga_out <- GA::ga(type="real-valued", fitness=function(p)-rss_fun_2(p),
                                lower=lower2, upper=upper2,
                                popSize=controlGA$popSize, maxiter=controlGA$maxiter,
                                run=controlGA$run, seed=controlGA$seed, monitor = FALSE)
                init2 <- slot(ga_out,"solution")[1,]; names(init2)<-c("Linf","k")
                form2 <- substitute(length ~ L0 + (Linf - L0)*(1 - exp(-k*age)),
                                    list(L0=L0_fixed))
                m2 <- minpack.lm::nlsLM(form2, data=data, start=as.list(init2),
                                        lower=lower2, upper=upper2, control=nlsLM_ctrl)
                build_deterministic_VB(m2, data, L0_fixed, nlsLM_ctrl)
            } else {
                rss_fun_3 <- function(par) {
                Linf <- par[1]; k <- par[2]; t0 <- par[3]
                pred <- Linf*(1 - exp(-k*(data$age - t0)))
                if (any(!is.finite(pred))) return(1e10)
                sum((data$length - pred)^2 * early_penalty(data$age, early_factor=5))
                }
                lower3 <- c(Linf=0.9*max(data$length), k=1e-6, t0=min(data$age)-max(data$age))
                upper3 <- c(Linf=1.5*max(data$length), k=1,      t0=0)
                ga_out <- GA::ga(type="real-valued", fitness=function(p)-rss_fun_3(p),
                                lower=lower3, upper=upper3,
                                popSize=controlGA$popSize, maxiter=controlGA$maxiter,
                                run=controlGA$run, seed=controlGA$seed, monitor = FALSE)
                init3 <- slot(ga_out,"solution")[1,]; names(init3)<-c("Linf","k","t0")
                minpack.lm::nlsLM(length ~ Linf*(1 - exp(-k*(age - t0))),
                                data=data, start=as.list(init3),
                                lower=lower3, upper=upper3, control=nlsLM_ctrl)
            }
            },
            
            # 3) Logistic
            Logistic = function(data) {
            L0 <- if (!is.null(lengthAtBirth)) lengthAtBirth else min(data$length)
            rss_fun <- function(par) {
                xmid <- par[1]; scal <- par[2]
                Asym <- L0*(1+exp(xmid/scal))
                pred <- Asym/(1+exp((xmid - data$age)/scal))
                if (any(!is.finite(pred))) return(1e10)
                sum((data$length - pred)^2 * early_penalty(data$age, early_factor=5))
            }
            lower <- c(xmid=min(data$age)/2, scal=1e-4)
            upper <- c(xmid=(max(data$age)+sd(data$age))/2, scal=diff(range(data$age)))
            ga_out <- GA::ga(type="real-valued", fitness=function(p)-rss_fun(p),
                            lower=lower, upper=upper,
                            popSize=controlGA$popSize, maxiter=controlGA$maxiter,
                            run=controlGA$run, seed=controlGA$seed, monitor = FALSE)
            init <- slot(ga_out,"solution")[1,]; names(init)<-c("xmid","scal")
            Asym_val <- L0*(1+exp(init["xmid"]/init["scal"]))
            form <- substitute(length ~ Asym/(1+exp((xmid - age)/scal)), list(Asym=Asym_val))
            minpack.lm::nlsLM(form, data=data,
                            start=as.list(init),
                            lower=lower, upper=upper, control=nlsLM_ctrl)
            },
            
            # 4) Logistic4P_New
            Logistic4P_New = function(data) {
            L0      <- if (!is.null(lengthAtBirth)) lengthAtBirth else min(data$length)
            Lmax    <- max(data$length); age_max <- max(data$age)
            rss_fun <- function(par) {
                xmid <- par[1]; scal <- par[2]; L_max <- par[3]; L0_ <- par[4]
                A <- (L_max + L_max*exp((xmid - age_max)/scal) - L0_ - L0_*exp(xmid/scal)) /
                    (exp((xmid - age_max)/scal) - exp(xmid/scal))
                B <- (L0_ - A)*(1+exp(xmid/scal)) + A
                pred <- A + (B - A)/(1+exp((xmid - data$age)/scal))
                if (any(!is.finite(pred))) return(1e10)
                sum((data$length - pred)^2 * early_penalty(data$age, early_factor=5))
            }
            lower <- c(xmid=min(data$age), scal=0.01, L_max=0.95*Lmax, L0=0.8*min(data$length))
            upper <- c(xmid=max(data$age), scal=diff(range(data$age)),L_max=1.05*Lmax, L0=1.2*min(data$length))
            ga_out <- GA::ga(type="real-valued", fitness=function(p)-rss_fun(p),
                            lower=lower, upper=upper,
                            popSize=controlGA$popSize, maxiter=controlGA$maxiter,
                            run=controlGA$run, seed=controlGA$seed, monitor = FALSE)
            init <- slot(ga_out,"solution")[1,]; names(init)<-c("xmid","scal","L_max","L0")
            A_expr <- paste0("(L_max + L_max*exp((xmid-",age_max,")/scal) - L0 - L0*exp(xmid/scal)) / ",
                            "(exp((xmid-",age_max,")/scal) - exp(xmid/scal))")
            B_expr <- paste0("(L0 - ",A_expr,")*(1+exp(xmid/scal)) + ",A_expr)
            form4 <- as.formula(paste0("length ~ ",A_expr," + (",B_expr," - ",A_expr,")/",
                                        "(1+exp((xmid - age)/scal))"))
            minpack.lm::nlsLM(form4,
                            data    = data,
                            start   = as.list(init),
                            lower   = init,
                            upper   = init,
                            control = nlsLM_ctrl)
            },
            
            # 5) Logistic4P_FixedA
            Logistic4P_FixedA = function(data) {

                A_fixed <- if (!is.null(lengthAtBirth)) lengthAtBirth else min(data$length)
                
                rss_fun <- function(par) {
                    xmid <- par[1]
                    scal <- par[2]
                    B <- par[3]
                    offset <- par[4]

                    pred <- (A_fixed - offset) + (B - (A_fixed - offset))/(1+exp((xmid - data$age)/scal))

                    if (any(!is.finite(pred))) return(1e10)

                    sum((data$length - pred)^2 * early_penalty(data$age, early_factor=5))
                }

                lower <- c(xmid=min(data$age), scal=0.01, B=0.95*max(data$length), offset=-0.1*A_fixed)
                upper <- c(xmid=max(data$age), scal=diff(range(data$age)),B=1.05*max(data$length), offset=2.0*A_fixed)

                ga_out <- GA::ga(type="real-valued", fitness=function(p)-rss_fun(p),
                                lower=lower, upper=upper,
                                popSize=controlGA$popSize, maxiter=controlGA$maxiter,
                                run=controlGA$run, seed=controlGA$seed, monitor = FALSE)

                init <- slot(ga_out,"solution")[1,]
                names(init)<-c("xmid","scal","B","offset")

                form4 <- as.formula(paste0("length ~ (", A_fixed, " - offset) + (B - (", A_fixed, " - offset)) / (1 + exp((xmid - age) / scal))"))

                minpack.lm::nlsLM(form4,
                                data    = data,
                                start   = as.list(init),
                                lower   = init,
                                upper   = init,
                                control = nlsLM_ctrl)
            },
            
            # 6) Linear with extra penalty for saturating data
            Linear = function(data) {
            rss_fun <- function(par) {
                pred <- par[1] + par[2]*data$age
                if (any(!is.finite(pred))) return(1e10)
                resid    <- data$length - pred
                weight   <- early_penalty(data$age, early_factor=5)
                base_rss <- sum((resid^2) * weight)
                # penaliza si hay saturación
                if ((max(data$length)-median(data$length)) < 0.1*(max(data$length)-min(data$length)) &&
                    abs(par[2]) > 1e-5) {
                base_rss + 1e6*abs(par[2])
                } else base_rss
            }
            lower <- c(a=0.8*min(data$length), b=-10)
            upper <- c(a=1.2*min(data$length), b=10)
            ga_out <- GA::ga(type="real-valued", fitness=function(p)-rss_fun(p),
                            lower=lower, upper=upper,
                            popSize=controlGA$popSize, maxiter=controlGA$maxiter,
                            run=controlGA$run, seed=controlGA$seed, monitor = FALSE)
            init <- slot(ga_out,"solution")[1,]; names(init)<-c("a","b")
            minpack.lm::nlsLM(length ~ a + b*age,
                                data    = data,
                                start   = as.list(init),
                                lower   = lower,
                                upper   = upper,
                                control = nlsLM_ctrl)
            }

        )  # end candidate_models
        




        


        #---------------------------------------------------------------------
        # If force_model is specified, return only that candidate.
        if (!is.null(force_model)) {
            if (!(force_model %in% names(candidate_models))) {
                stop("Invalid model name. Available models: ", paste(names(candidate_models), collapse = ", "))
            }
            
            cat("Forcing model:", force_model, "\n")

            forced_model <- tryCatch({
                    candidate_models[[force_model]](data)
                }, 
                error = function(e) {
                    stop("Failed to fit forced model '", force_model, "'. Reason: ", e$message)
                }
            )


            model_plots <- list()


            new_age <- seq(0, max(data$age), length.out = 100)
            newdata_for_prediction <- data.frame(age = new_age)
            

            if (use_app && "app" %in% ls(envir = parent.frame(2))) {

                pred_vals <- predict(forced_model, newdata = newdata_for_prediction)
                plt <- plot_ly() %>%
                    add_lines(x = new_age, y = pred_vals,
                            name = "Predicted", line = list(color = 'blue')) %>%
                    add_markers(x = data$age, y = data$length, name = "Observed", marker = list(color = 'red')) %>%
                    layout(title = paste("Model:", force_model),
                        xaxis = list(title = "Age"),
                        yaxis = list(title = "Length"))
                model_plots[[force_model]] <- plt

            } else {

                op <- par(mfrow = c(1, 1))
                
                pred_vals <- predict(forced_model, newdata = newdata_for_prediction)
                max_y <- max(data$length, pred_vals, na.rm = TRUE)
                plot(data$age, data$length, col = "red", pch = 16,
                    main = paste("Model:", force_model),
                    xlab = "Age", ylab = "Length",
                    xlim = c(0, max(data$age)),
                    ylim = c(0, max_y))
                lines(new_age, pred_vals, col = "blue")
                legend("bottomright", legend = c("Predicted", "Observed"),
                    col = c("blue", "red"), lty = c(1, NA), pch = c(NA, 16), cex = 0.7)
                
                model_plots[[force_model]] <- recordPlot()

                par(op)

            }



            A <- NULL

            if(force_model == "Logistic4P_New") {
                age_max <- max(data$age)

                xmid <- as.vector(t(coef(forced_model)["xmid"]))
                scal <- as.vector(t(coef(forced_model)["scal"]))
                L_max <- as.vector(t(coef(forced_model)["L_max"]))
                L0 <- as.vector(t(coef(forced_model)["L0"]))

                A <- (L_max + L_max*exp((xmid - age_max)/scal) - L0 - L0*exp(xmid/scal)) /
                    (exp((xmid - age_max)/scal) - exp(xmid/scal))
            } else if (force_model == "Logistic4P_FixedA") {
                A_fixed <- if (!is.null(lengthAtBirth)) lengthAtBirth else min(data$length)

                A <- (A_fixed - as.vector(t(coef(forced_model)["offset"])))
            }



            force_model <- if (force_model %in% c("Logistic4P_New", "Logistic4P_FixedA")) "Logistic4P" else force_model


            growthK <- NA
            if (force_model %in% c("Exponential", "VonBertalanffy")) {
                growthK <- as.vector(t(coef(forced_model)["k"]))
            } else if (force_model == "Linear") {
                growthK <- as.vector(t(coef(forced_model)["b"]))
            } else if (force_model %in% c("Logistic", "Logistic4P")) {
                scal <- as.vector(t(coef(forced_model)["scal"]))
                growthK <- 1 / scal
            } else {
                stop("Unknown forced model type.")
            }


            
            return(list(
                best_model_name = force_model,
                best_model = forced_model,
                growth = growthK,
                A = A,
                original_model_formula = NA,
                aic_values = list(force_model = forced_model$aic),
                aicc_values = list(force_model = forced_model$aicc),
                rss_values = list(force_model = forced_model$rss),
                all_models = list(force_model = list(model_name = force_model, fit = forced_model)),
                model_plots = model_plots,
                adjusted_models_table = NULL,
                pairwise_comparisons_table = NULL
            ))
        }




        #---------------------------------------------------------------------
        # Otherwise, fit all candidate models.
        results_list <- lapply(names(candidate_models), function(mn) {
            tryCatch({
                    fit_obj <- candidate_models[[mn]](data)
                    aic_val <- AIC(fit_obj)
                    aicc_val <- tryCatch(MuMIn::AICc(fit_obj), error = function(e) NA)
                    rss_val <- sum(residuals(fit_obj)^2)
                    list(model_name = mn, fit = fit_obj, aic = aic_val, aicc = aicc_val, rss = rss_val)
                }, 
                error = function(e) {
                    message("Model '", mn, "' failed. Reason: ", e$message)
                    NULL
                }
            )
        })

        results <- Filter(Negate(is.null), results_list)

        names(results) <- sapply(results, function(x) x$model_name)

        if (length(results) == 0)
            stop("All models failed to fit.")
        
        cat("Successfully fitted models:\n")

        for (r in results) cat(r$model_name, "\n")

        if ("VonBertalanffy" %in% names(results) && (!is.null(lengthAtBirth)|| has_age0(data))) 
            cat("Deterministic VB model built and used for VonBertalanffy candidate.\n")






        #---------------------------------------------------------------------
        # Automatic model selection.
        best_candidate <- NULL
        n_obs <- nrow(data)

        if (model_choose == "auto") {
            if (n_obs <= 10) {
                best_index <- which.min(sapply(results, function(x) x$rss))
                best_candidate <- results[[best_index]]
                cat("Best model (n <= 10) based on RSS:", best_candidate$model_name, "\n")
            } else if (n_obs <= 29) {
                best_index <- which.min(sapply(results, function(x) x$aicc))
                best_candidate <- results[[best_index]]
                cat("Best model (11 <= n <= 29) based on AICc:", best_candidate$model_name, "\n")
            } else {
                best_index <- which.max(sapply(results, function(x) as.numeric(logLik(x$fit))))
                best_candidate <- results[[best_index]]
                cat("Candidate with highest logLik:", best_candidate$model_name, "\n")
                not_signif <- best_index
                for (i in seq_along(results)) {
                    if (i != best_index) {
                    alt <- results[[i]]
                    LRT_stat <- 2 * abs(as.numeric(logLik(alt$fit)) - as.numeric(logLik(best_candidate$fit)))
                    df_diff <- abs(attr(logLik(best_candidate$fit), "df") - attr(logLik(alt$fit), "df"))
                    p_val <- if (df_diff == 0) 1 else pchisq(LRT_stat, df = df_diff, lower.tail = FALSE)
                    if (p_val >= 0.05)
                        not_signif <- c(not_signif, i)
                    }
                }
                candidate_dfs <- sapply(not_signif, function(i) attr(logLik(results[[i]]$fit), "df"))
                final_index <- not_signif[which.min(candidate_dfs)]
                best_candidate <- results[[final_index]]
                cat("Final best model (n >= 30):", best_candidate$model_name, "\n")
            }
        }





        #---------------------------------------------------------------------
        # Build grid plot for candidate models.
        model_plots <- list()


        n_models <- length(results)
        ncol_pl <- ceiling(sqrt(n_models))
        nrow_pl <- ceiling(n_models / ncol_pl)
        new_age <- seq(0, max(data$age), length.out = 100)
        newdata_for_prediction <- data.frame(age = new_age)
        

        if (use_app && "app" %in% ls(envir = parent.frame(2))) {

            for (res in results) {
                pred_vals <- predict(res$fit, newdata = newdata_for_prediction)
                plt <- plot_ly() %>%
                    add_lines(x = new_age, y = pred_vals,
                            name = "Predicted", line = list(color = 'blue')) %>%
                    add_markers(x = data$age, y = data$length, name = "Observed", marker = list(color = 'red')) %>%
                    layout(title = paste("Model:", res$model_name),
                        xaxis = list(title = "Age"),
                        yaxis = list(title = "Length"))
                model_plots[[res$model_name]] <- plt
            }

        } else {

            op <- par(mfrow = c(nrow_pl, ncol_pl))
            for (res in results) {
                pred_vals <- predict(res$fit, newdata = newdata_for_prediction)
                max_y <- max(data$length, pred_vals, na.rm = TRUE)
                plot(data$age, data$length, col = "red", pch = 16,
                    main = paste("Model:", res$model_name),
                    xlab = "Age", ylab = "Length",
                    xlim = c(0, max(data$age)),
                    ylim = c(0, max_y))
                lines(new_age, pred_vals, col = "blue")
                legend("bottomright", legend = c("Predicted", "Observed"),
                    col = c("blue", "red"), lty = c(1, NA), pch = c(NA, 16), cex = 0.7)
                
                model_plots[[res$model_name]] <- recordPlot()
            }
            par(op)

        }

            


        #---------------------------------------------------------------------
        # Build Pairwise Comparisons Table.
        pairwise_comparisons <- data.frame(
            Model1 = character(),
            Model2 = character(),
            p_value_model1_vs_model2 = character(),
            stringsAsFactors = FALSE
        )
        for (i in seq_len(n_models - 1)) {
            for (j in (i + 1):n_models) {
            m1 <- results[[i]]$fit
            m2 <- results[[j]]$fit
            loglik1 <- tryCatch(as.numeric(logLik(m1)), error = function(e) NA)
            loglik2 <- tryCatch(as.numeric(logLik(m2)), error = function(e) NA)
            df1 <- tryCatch(attr(logLik(m1), "df"), error = function(e) NA)
            df2 <- tryCatch(attr(logLik(m2), "df"), error = function(e) NA)
            if (!is.na(loglik1) && !is.na(loglik2) && !is.na(df1) && !is.na(df2)) {
                LRT_stat <- 2 * abs(loglik1 - loglik2)
                df_diff <- abs(df1 - df2)
                p_val <- if (df_diff == 0) 1 else pchisq(LRT_stat, df = df_diff, lower.tail = FALSE)
                p_val_fmt <- sprintf("%.2E", p_val)
            } else {
                p_val_fmt <- NA
            }
            pairwise_comparisons <- rbind(pairwise_comparisons, data.frame(
                Model1 = results[[i]]$model_name,
                Model2 = results[[j]]$model_name,
                p_value_model1_vs_model2 = p_val_fmt,
                stringsAsFactors = FALSE
            ))
            }
        }
        
        #---------------------------------------------------------------------
        # Build Adjusted Models Summary Table.
        summary_rows <- list()
        for (r in results) {
            is_best <- (r$model_name == best_candidate$model_name)
            loglik_val <- tryCatch(as.numeric(logLik(r$fit)), error = function(e) NA)
            df_val <- tryCatch(attr(logLik(r$fit), "df"), error = function(e) NA)
            if (!is_best && !is.na(df_val) && !is.null(best_candidate)) {
            LRT_stat <- 2 * abs(loglik_val - as.numeric(logLik(best_candidate$fit)))
            df_diff <- abs(df_val - attr(logLik(best_candidate$fit), "df"))
            p_val <- if (df_diff == 0) 1 else pchisq(LRT_stat, df = df_diff, lower.tail = FALSE)
            } else {
            p_val <- NA
            }
            p_val_fmt <- if (is.na(p_val)) NA else sprintf("%.2E", p_val)
            summary_rows[[length(summary_rows) + 1]] <- data.frame(
            Model = r$model_name,
            AIC = r$aic,
            AICc = r$aicc,
            RSS = r$rss,
            best_candidate = is_best,
            Loglik = loglik_val,
            df = df_val,
            p_value_LRT = p_val_fmt,
            stringsAsFactors = FALSE
            )
        }
        adjusted_models_table <- do.call(rbind, summary_rows)
        


        A <- NULL

        if(best_candidate$model_name == "Logistic4P_New") {
            age_max <- max(data$age)

            xmid <- as.vector(t(coef(best_candidate$fit)["xmid"]))
            scal <- as.vector(t(coef(best_candidate$fit)["scal"]))
            L_max <- as.vector(t(coef(best_candidate$fit)["L_max"]))
            L0 <- as.vector(t(coef(best_candidate$fit)["L0"]))

            A <- (L_max + L_max*exp((xmid - age_max)/scal) - L0 - L0*exp(xmid/scal)) /
                (exp((xmid - age_max)/scal) - exp(xmid/scal))
        } else if (best_candidate$model_name == "Logistic4P_FixedA") {
            A_fixed <- if (!is.null(lengthAtBirth)) lengthAtBirth else min(data$length)

            A <- (A_fixed - as.vector(t(coef(best_candidate$fit)["offset"])))
        }


        best_candidate$model_name <- if (best_candidate$model_name %in% c("Logistic4P_New", "Logistic4P_FixedA")) "Logistic4P" else best_candidate$model_name


        #---------------------------------------------------------------------
        # Extract the growth parameter from the best candidate.
        growthK <- NA
        if (!is.null(best_candidate)) {
            if (best_candidate$model_name %in% c("Exponential", "VonBertalanffy")) {
                growthK <- as.vector(t(coef(best_candidate$fit)["k"]))
            } else if (best_candidate$model_name == "Linear") {
                growthK <- as.vector(t(coef(best_candidate$fit)["b"]))
            } else if (best_candidate$model_name %in% c("Logistic", "Logistic4P")) {
                scal <- as.vector(t(coef(best_candidate$fit)["scal"]))
                growthK <- 1 / scal
            } else {
                stop("Unknown model type.")
            }
        }
        
        #---------------------------------------------------------------------
        # Return final output list.
        return(list(
            best_model_name = if (!is.null(best_candidate)) best_candidate$model_name else NA,
            best_model = if (!is.null(best_candidate)) best_candidate$fit else NULL,
            growth = growthK,
            A = A,
            original_model_formula = NA,
            aic_values = setNames(sapply(results, function(x) x$aic), names(results)),
            aicc_values = setNames(sapply(results, function(x) x$aicc), names(results)),
            rss_values = setNames(sapply(results, function(x) x$rss), names(results)),
            all_models = results,   # Access as: fitted$all_models$VonBertalanffy$fit or ...$VonBertalanffy_deterministic$fit.
            model_plots = model_plots,
            adjusted_models_table = adjusted_models_table,
            pairwise_comparisons_table = pairwise_comparisons
        ))

    }





    # ===========================
    # Begin main function.
    if (missing(x) || missing(y))
        stop("Column names for x and y must be provided.")



    for(temp in names(data)) {
        data[[temp]]$age <- data[[temp]][[x]]
        data[[temp]]$length <- data[[temp]][[y]]
    }



    if (use_app && is.null(force_model) && "app" %in% ls(envir = parent.frame())) {
        if("force_model" %in% names(parent.frame()$app)) {
            force_model <- parent.frame()$app$force_model
        }
    }


    results_all <- list()

    plots <- list()

    for(temp in sort(names(data))) {

        cat("Running models for temperature =", temp, "\n")

        temp_force_model <- NULL

        if(temp %in% names(force_model)) {
            temp_force_model <- force_model[[temp]]
        }


        results_all[[temp]] <- fit_data(
            data = data[[temp]],
            nlsMcontrol = nlsMcontrol, 
            controlGA = controlGA, 
            force_model = temp_force_model, 
            model_choose = model_choose, 
            lengthAtBirth = lengthAtBirth, 
            lower = lower
        )

        results_all[[temp]]$temperature = as.numeric(temp)

        plots[[temp]] <- results_all[[temp]]$model_plots

    }

    if (use_app && "app" %in% ls(envir = parent.frame())) {
        assign("growth_curve_plots", plots, envir = parent.frame())
    }

    return(results_all)

}
