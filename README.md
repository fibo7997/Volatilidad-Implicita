# Volatilidad-Implicita

Este programa busca calcular la volatilidad implicita de una serie de tiempo de calls de Galicia. 

La volatilidad implicita es aquella que esta representada por el precio del call. Es decir, lo que el mercado esta sobre especulando sobre el activo. Estos swings de volatilidad se dan en un contexto electoral, en donde el riesgo de Galicia es representado por el riesgo del pais (aparte de que el beta de galicia suele rondar 1). 

Este trabajo demuestra empiricamente lo que es estudiado en la teoria, la volatilidad implicita siempre revierte a la media (o a su volatilidad realizada)

Estrategias de opciones: Como se pueden ver en los graficos (), esta brecha entre la volatilidad realizada y la implicita representa una oportunidad de trade.

Se podria shortear calls y puts y delta hedgear la posicion. Los puntos de entrada (imagen 3), muestran los puntos en donde se puede inicar la venta de estas opciones para aporvechar la compresion de esta brecha. En caso de que la volatilidad implicita siga subiendo se puede aumentar la posicion (por ejemplo empezar cuando la volatilidad implicita este en 70% y aumentar la exposicion cuando este en 150%)
