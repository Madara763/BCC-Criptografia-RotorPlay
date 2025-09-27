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
Basicamente eh uma mistura de [PlayFair].(https://en.wikipedia.org/wiki/Playfair_cipher) com [Cifra de Rotor].(https://en.wikipedia.org/wiki/Rotor_machine)
Para mais detalhes, leia o codigo.
