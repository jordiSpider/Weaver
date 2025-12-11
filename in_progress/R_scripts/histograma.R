

datos <- read.table("/home/macarse/Escritorio/EEZA/Weaver/output/first_test_Amblyseius_swirskii_new__2025-1-30_20-0-31/animal_constitutive_traits/Amblyseius_swirskii.txt", header = TRUE, sep = "\t", stringsAsFactors = FALSE)


colnames(datos)

hist(datos$longevitySinceMaturation, breaks = 100, col = "blue", main = "Histograma",
     xlab = "Value", xlim = c(95, 115))



shapiro.test(datos$longevitySinceMaturation)


