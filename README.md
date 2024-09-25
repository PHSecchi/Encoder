# Encoder

Este projeto foi desenvolvido na disciplina de Sensores do curso de Tecnologia em Mecatrônica Industrial, no Instituto Federal de São Paulo, campus Catanduva.

O objetivo deste trabalho foi a criação de um sensor de velocidade de rotação utilizando um encoder. O encoder consiste em uma chapa circular com furos equidistantes próximos às bordas, fixada em um eixo. Um sensor infravermelho é posicionado de tal forma que o feixe entre o receptor e o emissor passe pelos furos. Ao acelerar o disco, é possível contar quantas vezes o sensor é acionado em um determinado intervalo de tempo. Sabendo o número de furos presentes no disco, podemos calcular as rotações do eixo.

O código foi escrito em linguagem C e utiliza o microcontrolador ATmega328P, o mesmo empregado no Arduino Uno. Contudo, diferentemente do ambiente Arduino IDE, que oferece uma programação mais simplificada, neste projeto a programação foi realizada diretamente no microcontrolador, utilizando registradores para a configuração de entradas e saídas, temporizadores, prescalers, além de leituras para diagnóstico e aferição de eventuais problemas.

Para mais detalhes e esclarecimentos, recomenda-se consultar o relatório disponível no repositório.