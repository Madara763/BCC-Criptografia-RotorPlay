# BCC-Criptografia-RotorPlay
Esse repositorio contem um algoritomo de criptografia criado como parte do trabalho de criptografia de BCC da UFPR

## Compilar
```
  Make
```
## Executar
```
Exemplos:
  ./cripto -c -e in -s out -k chave
  ./cripto -d -e cifrado -s saida -k chave
  ./cripto -g -s chave
```

## Informacoes sobre o programa
```
Esse programa recebe algums parametros na linha de comando:
  -d : indica que o programa vai decriptar, espera -e <param> -s <param> e -k <param>
  -c : indica que o programa vai cifrar , espera -e <param> -s <param> e -k <param>
  -g : indica que o programa vai gerar um arquivo com o uma chave aleatoria, espera -s <param>

  Onde <param> sao os nomes dos arquivos de texto.

Opcoes complementares:
  -e : indica o arquivo de entrada.
  -s : indica o arquivo de saida. Se existir sera sobreescrito, se nao existir cria um no diretorio corrente.
  -k : indica que o proximo parametro contem o nome do arquivo com a chave que sera usada na cifra, gera erro se o arquivo nao existir ou se a chave nao estiver no formato esperado.
```

## Informacoes sobre o algoritmo
Basicamente eh uma mistura de [PlayFair](https://en.wikipedia.org/wiki/Playfair_cipher) com [Cifra de Rotor](https://en.wikipedia.org/wiki/Rotor_machine).  

### Detalhes:
Para enterdermos o RotorPLay é necessario um bom entendimento de como a PlayFair e como a Cifra de Rotor funcionam.  

#### [PLAYFAIR](https://en.wikipedia.org/wiki/Playfair_cipher):  
Uma cifra de substituição que usa uma matriz de caracteres como chave.  
A cifra é feita com base em digramas, onde pares de letras são utilizados para busca e substituição na matriz, ou seja, dado um digrama [X,Y] é feita a busca por linha e coluna na matriz e gera um digrama de saida [A,B].  
Em uma matriz 5x5, com 25 caracteres como a usada na PLayFair, cada caractere tem mais de 600 combinacoes que geram a saida, esse numero é dado por $N^2$ onde N = Quantidade de posições da matriz.  

#### [CIFRA DE ROTOR](https://en.wikipedia.org/wiki/Rotor_machine):  
Uma cifra de substituição e de fluxo que utiliza varios discos alinhados para troca dos caracteres.  
A cifra é feita da seguinte forma, cada disco faz uma substituição, para um caractere na entrada, gera um caractere na saída, que por sua vez se torna o caractere de entrada do próximo disco, e assim até passar por todos os discos. Quando passa por todos os discos o primeiro disco é girado em uma posição, fazendo com que ao inserir o mesmo caractere da rodada anterior a saída seja diferente. Quando o primeiro disco completa uma volta, o próximo disco gira uma posição, igual um odômetro de carro.  
Essa cifra gera uma explosão combinatória a cada novo disco, e a quantidade de possibilidades para cada caractere é dada pela seguinte fórmula $C^D$ onde C = Quantidade de Caracteres em cada disco e D = Número de discos.  

#### ROTORPLAY:  
Conhecendo a PlayFair e a Cifra de Rotor podemos explicar a RotorPlay já que é exatamente uma combinação das duas.  
A estrutura da RotorPlay é abstraída para um cubo de 16x16x16 Bytes, ou 16 matrizes de 16x16 Bytes, onde podemos entender cada matriz como uma matriz da PlayFair e também cada matriz é um disco da Cifra de Rotor e tem 16 discos.  
Para explicar a RotorPlay precisamos definir uma camada, chamamos de camada as matrizes do cubo, ou seja, um cubo de 16x16x16 Bytes é composto por 16 camadas, e cada camada corresponde a uma matriz de 16x16 Bytes, ou seja, cada camada tem 256 posições, cada uma com um byte.  
Agora, vamos a cifra da RotorPlay, em um nivel macro, podemos ver como uma Cifra de Rotor, onde cada camada funciona como um disco, recebe a unidade de dados, substitui e gera uma saída, que por sua vez entra em outra camada e assim sucessivamente.  
A unidade de dados supracitada é um par de bytes, semelhante a PlayFair, mas ao ínves de letras, usamos bytes.   
Em um nivel macro cada camada do cubo funciona como uma PlayFair, onde o par de bytes é usado na consulta das posições na matriz e cada dois bytes são trocados por outros dois bytes.  
Agora para finalizar, a rotação usada na Cifra de Rotor que é o coração da cifra, na RotorPlay é feita como um deslocamento dos bytes dentro da camada, onde o byte no índice n após uma rotação vai para o índice n+1, e o da última posição vai para a primeira, como uma lista circular, após 256 rotações a primeira camada terá completado uma volta e a segunda camada fará a sua primeira rotação. 

##### Chave:
A chave da RotorPlay é composta 16 sequências de bytes sem repetição tamanho variavel de 0 a 256, onde cada sequência será utilizada para preencher cada uma das 16 camadas da mesma forma que a PlayFair, os bytes são copiados para o inicio da matriz e se não completar a matriz é completado em ordem crescente sem que haja repetição.  
A opção -g do programa gera chaves completas, com 16 sequências de 256 bytes.  

##### Caracteristicas:
Poder de criptografia: A cada camada o par de bytes segue a mesma regra de possibilidades que a PlayFair, ou seja, cada par de bytes tem $256^2$ possibilidades de substituição. A quantidade total de possibilidades de substituição segue a regra da Cifra de Rotor, $C^D$ onde C = Quantidade de Caracteres em cada disco e D = Número de discos, mas nesse caso C = $256^2$ já que temos uma combinatória em cada camada, resumindo, na RotorPlay, cada par de bytes tem $(256^2)^C$ -> $(256^2)^16$ -> $((2^8)^2)^16$

### Definicoes:

Para mais detalhes, leia o codigo.
