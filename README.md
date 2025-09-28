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

#### [PlayFair](https://en.wikipedia.org/wiki/Playfair_cipher):
Uma cifra de substituição que usa uma matriz de caracteres como chave.  
A cifra é feita com base em digramas, onde pares de letras são utilizados para busca e substituição na matriz, ou seja, dado um digrama [X,Y] é feita a busca por linha e coluna na matriz e gera um digrama de saida [A,B].  


### Definicoes:

Para mais detalhes, leia o codigo.
