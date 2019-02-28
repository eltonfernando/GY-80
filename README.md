# GY-80
  Unidade inercial com fusão de sensor por filtro complementar otimizado para alto desempenho. O filtro funde o acelerômetro adlx354 e o giroscópio L3G4200D.
 
## Giroscópio
  O giroscópio mede velocidade, para obter a posição é necessario fazer uma integração, isso gera um desvio, porém em movimento apresenta uma ecelente medidas. 
## Acelerómetro
  O Acelerómetro tem boas medições em posição estática, porém quando há movimento a saída do sensor sofre com ruídos.
 # Filtro complementar
   O filtro complementar reúne o melhor dos dois sensores, em que as informações redundantes contribuem para redução do erro e o complemento melhora a observabilidade, permitindo obter informação de certos parâmetros que não seria possível com sensor individual.
  
