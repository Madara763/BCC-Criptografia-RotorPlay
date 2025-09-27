#include <unordered_map>
#include <vector>
#include <cstdint>
#include <iostream>
#include <algorithm> 
#include <random>    
#include <ctime> 
#include <chrono>    
#include <fstream>
#include <filesystem>

//----------------------DEFINES----------------------
#define TAM_CHAVE_PADRAO 256
#define QUANT_CHAVE_PADRAO 16
#define LINHAS_CHAVE_PADRAO 16
#define COLUNAS_CHAVE_PADRAO 16


//----------------------STRUCTS----------------------
//camada de tamanho fixo em 256
//representa uma das matrizes do cubo, de 16x16
typedef struct camada_256_t{
  
  std::unordered_map<uint8_t, uint8_t> mapa; //Mapeia o byte para seu indice inicial
  uint8_t vetor[256]; //Armazena os bytes
  uint8_t count{0}; //Conta quantos "giros" essa camada ja fez
  
} camada_256_t;

//Cubo contendo as varias dimensoes
typedef struct cubo_t{
  std::vector<camada_256_t*> camadas; 
  uint64_t profundidade; //Quantas camadas o cubo possui
} cubo_t;

//Chave completa
typedef struct chave_t{
  std::vector<std::vector<uint8_t>> c;
  uint64_t rfk{0};
} chave_t;

//----------------------FUNCOES----------------------

/*
  Cria o "motor" da criptografia.
  Recebe a dimencao do cubo e as chaves, inicializa as estruturas
  Devolve ...
*/
cubo_t* inicializa_cubo(const chave_t& chave, const int tam_max_chave, const uint64_t quant_camadas);

//Libera toda memoria alocada na estrutura
void destroi_cubo(cubo_t* cubo);

/*
  Gera um vetor de chaves
*/
chave_t* gera_chave();

/*
  Libera memoria da chave
*/
void destroi_chave(chave_t* chave);

/*
  Funcoes para ler e escrever a chave de um arquivo binario
  Cria um arquivo com a chave a partir de um chave_t
*/
void salvar_chave(std::ostream& saida, const chave_t& chave);

/*
  Cria um chave_t a partir de um arquivo
*/
chave_t* carregar_chave(std::istream& entrada);

/*
  Chama a criptografia nos bytes
  Recebe os streams para os arquvio de entrada e saida, e o cubo com as chaves
  Lanca excecoes
  Retorna 1 se deu tudo certo e 0 se houve erro
*/
int criptografa(std::istream& arq_claro, std::ostream& arq_cifrado, cubo_t* cubo, bool& padding);

/*
  Chama a descriptografia nos bytes
  Recebe os streams para os arquvio de entrada e saida, e o cubo com as chaves
  Lanca excecoes
  Retorna 1 se deu tudo certo e 0 se houve erro
*/
int descriptografa(std::istream& arquivo_cifrado, std::ostream& arquivo_claro, cubo_t* cubo, bool& padding);

/*
  Usado na cifra
  Recebe o cubo e os bytes a serem trocados
  Realiza a troca, incrementa as rotacoes e devolve os bytes no buffer que recebeu
*/
void troca_bytes(cubo_t* cubo, std::vector<uint8_t>& buffer);

/*
  Usado na decifra
  Recebe o cubo e os bytes a serem trocados
  Realiza a troca, incrementa as rotacoes e devolve os bytes no buffer que recebeu
*/
void destroca_bytes(cubo_t* cubo, std::vector<uint8_t>& buffer);
