#include "rotorplay.hpp" 

//----------------------DEFINES----------------------
/*
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
*/

//----------------------FUNCOES INTERNAS----------------------
//Retornam linha e coluna do byte na matriz da camada
//As matrizes sao representadas por vetor, isso requer um calculo para ter essas posicoes    
inline uint64_t linha_camada(camada_256_t* camada, const uint8_t byte){
  uint8_t posicao_ini = camada->mapa[byte];
  uint8_t posicao_atual = posicao_ini + camada->count; //Posicao deslocada pelas rotacoes
  posicao_atual = posicao_atual/LINHAS_CHAVE_PADRAO; //divisao inteira retorna a linha
  return posicao_atual;
}
inline uint64_t coluna_camada(camada_256_t* camada, const uint8_t byte){
  uint8_t posicao_ini = camada->mapa[byte];
  uint8_t posicao_atual = posicao_ini + camada->count; //Posicao deslocada pelas rotacoes
  posicao_atual = posicao_atual % COLUNAS_CHAVE_PADRAO; //Resto da divisao retorna a coluna
  return posicao_atual;
}
//Acessa o vetor na posicao ind (considera as rotacoes)
inline uint8_t acessa_vetor_chave(camada_256_t* camada,const uint8_t ind){
  uint8_t pos_final = ind - camada->count;
  return camada->vetor[pos_final];
}
//"Shift"a as posicoes dentro dos vetores das camadas  no cubo
void rotaciona_cubo(cubo_t* cubo){
  uint64_t tot_camadas=cubo->profundidade;
  uint64_t i=0;
  while(i<tot_camadas){
    if(cubo->camadas[i]->count == 255){
      //std::cout<<"Camada["<<i<<"]: "<<(int)cubo->camadas[i]->count<<" -> ";
      cubo->camadas[i]->count=0;
      i++;
      //std::cout<<(int)cubo->camadas[i]->count<<"\n";
    }
    else{
      //std::cout<<"Camada["<<i<<"]: "<<(int)cubo->camadas[i]->count<<" -> ";
      cubo->camadas[i]->count++;
      //std::cout<<(int)cubo->camadas[i]->count<<"\n";
      i=tot_camadas; //sai do laco
    }
  }
}
//Funcao para gerar um inteiro entre min e max
uint8_t gerar_byte_aleatorio(){
  uint8_t min{0}, max{255};
  std::mt19937 gen(std::random_device{}());
  std::uniform_int_distribution<> distrib(min, max);

  return distrib(gen);
}
//----------------------FUNCOES----------------------

/*
  Cria o "motor" da criptografia.
  Recebe a dimencao do cubo e as chaves, inicializa as estruturas
  Devolve ...
*/
cubo_t* inicializa_cubo(const chave_t& chave, const int tam_max_chave, const uint64_t quant_camadas){
  
  cubo_t *cubo{new cubo_t}; //Aloca o cubo
  cubo->profundidade=quant_camadas; //profundidade do cubo/num de camadas
  
  for(uint64_t i=0; i<quant_camadas; i++){ //aloca cada matriz do cubo
    try{
      cubo->camadas.push_back(new camada_256_t);
      cubo->camadas[i]->count=0;
    }
    catch(const std::exception& e)
    {
      std::cerr << e.what() << '\n';
    }
  }

  //Inicializa cada camada com a sua chave
  for(uint64_t num_chave=0; num_chave<chave.c.size(); num_chave++){
    int tam=chave.c[num_chave].size();
    
    //Coloca a chave no vetor
    for(int i=0; i<tam; i++){
      //inicia o vetor da dimesao(matriz)
      cubo->camadas[num_chave]->vetor[i] = chave.c[num_chave][i];

      //mapeia o byte para o indice
      //Significa que o byte tal esta inicialmente no indice i
      cubo->camadas[num_chave]->mapa[chave.c[num_chave][i]]=i;     
    }

    //Completa o vetor
    uint8_t byte{0};
    int i=tam;
    while(i<tam_max_chave){
      if(!cubo->camadas[num_chave]->mapa.contains(byte)){ //se esse byte nao estava na chave
        cubo->camadas[num_chave]->vetor[i]=byte;
        i++;
      } 
      //Se o byte ja estava na chave, so pula para achar um que nao esta na chave
      byte++; //pula para inserir o proximo byte
    }

    //Aqui ja completou uma "dimensao do cubo"
    //Vai para a proxima chave[num_chave] e coloca essa chave na segunda matriz
  }
  
  return cubo;
}

//Libera toda memoria alocada na estrutura
void destroi_cubo(cubo_t* cubo){
  if(!cubo)
    return;
  
  for(uint64_t i=0; i< cubo->camadas.size(); i++){
    delete cubo->camadas[i];
    cubo->camadas[i]=nullptr;
  }
  
  cubo->profundidade=0;
  delete(cubo);

  return;
}

/*
  Gera um vetor de chaves
*/
chave_t* gera_chave(){
  
  //Define o limite do intervalo e o vetor de chaves
  int limite_superior = TAM_CHAVE_PADRAO;
  chave_t* vetor_chaves=new chave_t;
  
  for(uint64_t chave=0; chave<QUANT_CHAVE_PADRAO; chave++){  
    //Cria um vetor com os numeros de 0 ate o lim sup
    std::vector<uint8_t> bytes_chave;
    for (int i = 0; i < limite_superior; ++i) {
      bytes_chave.push_back(i);
    }

    //inicializa o gerador de numeros aleatorios e faz umas combincoes
    std::random_device rd;
    auto chrono_seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::seed_seq seeder{rd(), static_cast<unsigned int>(chrono_seed), };
    
    std::mt19937 gerador(seeder); // Mersenne Twister

    //embaralha o vetor
    std::shuffle(bytes_chave.begin(), bytes_chave.end(), gerador);
    
    //adiciona o vetor de bytes/chave criado no vetor de chaves
    vetor_chaves->c.push_back(bytes_chave);
  }

  //Gera a chave da Railfence
  vetor_chaves->rfk=0;

  return vetor_chaves;
}


//Libera memoria da chave
void destroi_chave(chave_t* chave){
  if(!chave)  
    return;

  chave->rfk=0;

  for(uint64_t i=0; i<chave->c.size(); i++){
    for(uint64_t j=0; j<chave->c[i].size(); j++)
      chave->c[i][j]=0;
  }
  chave->c.clear();
  delete chave;
  
  return;
}

//Cria um arquivo com a chave a partir de um chave_t
void salvar_chave(std::ostream& saida, const chave_t& chave) {
  //Garante que o stream esta pronto para escrita
  if (!saida.good()) {
    throw std::runtime_error("Stream de saida nao esta pronto para escrita.");
  }

  //Escreve o tamanho do vetor externo c como um inteiro de 8 bytes (uint64_t)
  uint64_t outer_size = static_cast<uint64_t>(chave.c.size());
  saida.write(reinterpret_cast<const char*>(&outer_size), sizeof(outer_size));

  //Itera por cada vetor interno para salvá-lo
  for (const auto& inner_vec : chave.c) {
    //Escreve o tamanho do vetor interno como um inteiro de 8 bytes
    uint64_t inner_size = static_cast<uint64_t>(inner_vec.size());
    saida.write(reinterpret_cast<const char*>(&inner_size), sizeof(inner_size));

    //Escreve todos os bytes do vetor interno de uma vez
    saida.write(reinterpret_cast<const char*>(inner_vec.data()), inner_size);
  }
  
  //Escreve o byte no final como append 
  chave.rfk;

  //Verifica se ocorreu algum erro durante a escrita
  if (!saida.good()) {
    throw std::runtime_error("Erro ao escrever dados no stream.");
  }
}

//Cria um chave_t a partir de um arquivo
chave_t* carregar_chave(std::istream& entrada) {
  if (!entrada.good()) {
    throw std::runtime_error("Stream de entrada nao esta pronto para leitura.");
  }

  chave_t* chave_carregada{new chave_t};
  
  //Le o tamanho do vetor externo (8 bytes)
  uint64_t outer_size;
  entrada.read(reinterpret_cast<char*>(&outer_size), sizeof(outer_size));
  
  chave_carregada->c.reserve(outer_size);

  //Itera para ler cada vetor interno
  for (uint64_t i = 0; i < outer_size; ++i) {
    //Le o tamanho do próximo vetor interno (8 bytes)
    uint64_t inner_size;
    entrada.read(reinterpret_cast<char*>(&inner_size), sizeof(inner_size));

    //Cria um vetor interno com o tamanho correto
    std::vector<uint8_t> inner_vec(inner_size);

    //Le o bloco de bytes diretamente para a memoria do vetor interno
    if (inner_size > 0) {
      entrada.read(reinterpret_cast<char*>(inner_vec.data()), inner_size);
    }
    
    //Adiciona o vetor interno ao vetor principal
    chave_carregada->c.push_back(std::move(inner_vec));
  }
  
  //Ler o byte do railfence
  chave_carregada->rfk=0;

  if (entrada.fail() && !entrada.eof()) {
    throw std::runtime_error("Erro ao ler dados do stream.");
  }

  return chave_carregada;
}

/*
  Executa a criptografia
  Recebe os streams para os arquvio de entrada e saida, e o cubo com as chaves
  Lanca excecoes
  Retorna 1 se deu tudo certo e 0 se houve erro
*/
int criptografa(std::istream& arq_claro, std::ostream& arq_cifrado, cubo_t* cubo, bool& padding){
  if(!arq_claro.good() || !arq_cifrado.good()){
    throw std::runtime_error("Streams nao estao prontos para criptografia.");
  }

  //Buffer para armazenar os 2 bytes a serem processados
  std::vector<uint8_t> buffer;
  buffer.reserve(2); //reserva espaço para 2 bytes

  char byte_char;
  while (arq_claro.get(byte_char)){
    buffer.push_back(static_cast<uint8_t>(byte_char));

    //Se o buffer estiver cheio, processe e escreva
    if (buffer.size() == 2) {

      //PROCESSA
      //std::cout<<"Cifrando ["<<(int) buffer[0]<<","<<(int) buffer[1]<<"] -> ";
      troca_bytes(cubo, buffer);
      //std::cout<<"["<<(int) buffer[0]<<","<<(int) buffer[1]<<"]\n";

      //Apos fazer as trocas, rotaciona
      rotaciona_cubo(cubo);
    
      //Escreve os 2 bytes processados
      arq_cifrado.write(reinterpret_cast<const char*>(buffer.data()), 2);

      //Limpa o buffer
      buffer.clear();
    }
  }

  //arquivo com tamanho impar, adicionou 1 byte e o arquivo acabou
  if (!buffer.empty()) {  
    buffer.push_back(gerar_byte_aleatorio()); // Adiciona o padding 

    //PROCESSA
    //std::cout<<"Cifrando ["<<(int) buffer[0]<<","<<(int) buffer[1]<<"] -> ";
    troca_bytes(cubo, buffer);
    //std::cout<<"["<<(int) buffer[0]<<","<<(int) buffer[1]<<"]\n";

    //Apos fazer as trocas, rotaciona
    rotaciona_cubo(cubo);

    //Escreve o bloco final no arquivo cifrado
    arq_cifrado.write(reinterpret_cast<const char*>(buffer.data()), 2);
    
    //Avisa que teve que completar o arquivo
    padding=true;
  }

  // Verifica se ocorreu algum erro durante a escrita no stream de saída
  if(!arq_cifrado.good()){
    throw std::runtime_error("Erro ao escrever no stream de saida.");
  }
  return 1;
}


/*
  Usado na cifra
  Recebe o cubo e os bytes a serem trocados
  Realiza a troca, incrementa as rotacoes e devolve os bytes no buffer que recebeu
*/
void troca_bytes(cubo_t* cubo, std::vector<uint8_t>& buffer){
  //Bytes de entrada
  uint8_t b1 = buffer[0]; 
  uint8_t b2 = buffer[1]; 
  uint8_t s1, s2; //bytes da saida

  uint64_t total_camadas = cubo->profundidade; //cubo->profundidade;
  uint64_t lin, col, ind; //auxiliares

  //Percorre todas as camadas do cubo
  for(uint64_t n_camada=0; n_camada<total_camadas; n_camada++ ){
    //Faz a troca em uma camada
    
    //linha b1 e coluna b2
    lin=linha_camada(cubo->camadas[n_camada], b1);
    col=coluna_camada(cubo->camadas[n_camada], b2);
    ind = (lin * COLUNAS_CHAVE_PADRAO) + col;
    s1 = acessa_vetor_chave(cubo->camadas[n_camada], ind);


    //linha b2 e coluna b1
    lin=linha_camada(cubo->camadas[n_camada], b2);
    col=coluna_camada(cubo->camadas[n_camada], b1);
    ind = (lin * COLUNAS_CHAVE_PADRAO) + col;
    s2 = acessa_vetor_chave(cubo->camadas[n_camada], ind);

    //Troca os bytes para serem trocados na proxima camada
    b1=s1;
    b2=s2;
  }

  //Apos todas as trocas, retorna os bytes finais
  buffer[0]=s1;
  buffer[1]=s2;
  return;
}


/*
  Chama a descriptografia nos bytes
  Recebe os streams para os arquvio de entrada e saida, e o cubo com as chaves
  Lanca excecoes
  Retorna 1 se deu tudo certo e 0 se houve erro
*/
int descriptografa(std::istream& arq_cifrado, std::ostream& arq_claro, cubo_t* cubo, bool& padding){
  if(!arq_claro.good() || !arq_cifrado.good()){
    throw std::runtime_error("Streams nao estao prontos para criptografia.");
  }

  //Le o primeiro byte do arquivo que contem flags
  char flags;
  arq_cifrado.get(flags);

  //Buffer para armazenar os 2 bytes a serem processados
  std::vector<uint8_t> buffer;
  buffer.reserve(2); //reserva espaço para 2 bytes

  char byte_char;
  while (arq_cifrado.get(byte_char)){ 
    buffer.push_back(static_cast<uint8_t>(byte_char));

    //Se o buffer estiver cheio, processe e escreva
    if (buffer.size() == 2) {

      //PROCESSA
      //std::cout<<"Decifrando ["<<(int) buffer[0]<<","<<(int) buffer[1]<<"] -> ";
      destroca_bytes(cubo, buffer);
      //std::cout<<"["<<(int) buffer[0]<<","<<(int) buffer[1]<<"]\n";

      //Apos fazer as trocas, rotaciona
      rotaciona_cubo(cubo);
      
      //Escreve os 2 bytes processados
      arq_claro.write(reinterpret_cast<const char*>(buffer.data()), 2);

      //Limpa o buffer
      buffer.clear();
    }
  }
  
  //O uso do padding na cifra garante que o tamanho do arquivo eh par
  //A flag diz se tem o padding ou nao, se tiver temos que remover ele no final
  if(flags & 0b00000001)
    padding=true;

  // Verifica se ocorreu algum erro durante a escrita no stream de saída
  if(!arq_claro.good()){
    throw std::runtime_error("Erro ao escrever no stream de saida.");
  }
  return 1;
}

/*
  Usado na decifra
  Recebe o cubo e os bytes a serem trocados
  Realiza a troca, incrementa as rotacoes e devolve os bytes no buffer que recebeu
*/
void destroca_bytes(cubo_t* cubo, std::vector<uint8_t>& buffer){
  //Bytes de entrada
  uint8_t b1 = buffer[0]; 
  uint8_t b2 = buffer[1]; 
  uint8_t s1, s2; //bytes da saida

  uint64_t total_camadas = cubo->profundidade; //cubo->profundidade;
  uint64_t lin, col, ind; //auxiliares

  //Percorre todas as camadas do cubo DE TRAS PARA FRENTE
  for(int64_t n_camada=total_camadas-1; n_camada>=0; n_camada-- ){
    //Faz a troca em uma camada
    
    //linha b1 e coluna b2
    lin=linha_camada(cubo->camadas[n_camada], b1);
    col=coluna_camada(cubo->camadas[n_camada], b2);
    ind = (lin * COLUNAS_CHAVE_PADRAO) + col;
    s1 = acessa_vetor_chave(cubo->camadas[n_camada], ind);


    //linha b2 e coluna b2
    lin=linha_camada(cubo->camadas[n_camada], b2);
    col=coluna_camada(cubo->camadas[n_camada], b1);
    ind = (lin * COLUNAS_CHAVE_PADRAO) + col;
    s2 = acessa_vetor_chave(cubo->camadas[n_camada], ind);

    //Troca os bytes para serem trocados na proxima camada
    b1=s1;
    b2=s2;
  }

  //Apos todas as trocas, retorna os bytes finais
  buffer[0]=s1;
  buffer[1]=s2;
  return;
}