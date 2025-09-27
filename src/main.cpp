#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <cstdint>
#include <fstream>
#include <time.h>

#include "rotorplay.hpp"

using namespace std;

//Funcao auxiliar para processar argumentos e despuluir a main
int processa_arg(int argc, char** argv, string& cifrado, string& claro, string& chave);

int main(int argc, char **argv){
  
  //Declara variaveis
  string cifrado_nome, claro_nome, chave_nome;
  cubo_t* cubo{nullptr};
  chave_t* chave_estrutura{nullptr};
  int opt = processa_arg(argc, argv, cifrado_nome, claro_nome, chave_nome); //Processa entrada
  bool padding{false};
  uint8_t flags{0};

  //Se for cifra o decifra, carrega a chave
  //Esse trecho eh necessario no switch 1 e 2
  if(opt == 1 || opt == 2){

    //Abre o arquivo para ler
    ifstream arquivo_chave(chave_nome, ios::binary);
    if (!arquivo_chave) { cerr << "Erro ao abrir o arquivo para leitura.\n"; return 1;}

    //Carrega a chave
    try {chave_estrutura = carregar_chave(arquivo_chave);} 
    catch (const std::runtime_error& e) { cerr << "Excecao ao carregar: " << e.what() << "\n"; return 1;}
    
    arquivo_chave.close();
    cubo=inicializa_cubo(*(chave_estrutura), TAM_CHAVE_PADRAO, QUANT_CHAVE_PADRAO );
  }

  switch (opt)
  {
  case 1:{ //Cifrar
    //Abre os arquivos
    ifstream arquivo_claro(claro_nome, ios::binary); //Claro
    ofstream arquivo_cifrado(cifrado_nome, ios::binary | ios::out); //Cifrado, sobreescreve caso exista
    if(!arquivo_cifrado || !arquivo_claro){ cerr << "Erro ao abrir o arquivo.\n"; return 1; }

    //Reserva o primeiro byte para flags
    arquivo_cifrado.write(reinterpret_cast<const char*>(&flags), sizeof(flags));

    //Cifra
    try{
      criptografa(arquivo_claro, arquivo_cifrado, cubo, padding);
      
      //Move o pronteiro para o inicio do arquivo
      arquivo_cifrado.seekp(0, ios::beg);

      //Cria as flags
      if(padding) flags=0b00000001;

      //Salva as flags
      arquivo_cifrado.write(reinterpret_cast<const char*>(&flags), sizeof(flags));

      //Fecha os arquivos
      arquivo_claro.close();
      arquivo_cifrado.close();
    }catch (const filesystem::filesystem_error& e) {
      cerr << "Erro ao criptografar: " << e.what() << std::endl;
    }

    //Imprime saida
    cout<<"'"<<claro_nome<<"' cifrado em '"<<cifrado_nome<<"' com sucesso.\n";
    string comando_cksum="cksum "+cifrado_nome;
    system(comando_cksum.data());

    break;
  }
  case 2:{ //Decifrar
    ifstream arquivo_cifrado(cifrado_nome, ios::binary); //Cifrado
    ofstream arquivo_claro(claro_nome, ios::binary | ios::out); //Claro, sobreescreve caso exista
    if(!arquivo_cifrado || !arquivo_claro){ cerr << "Erro ao abrir o arquivo.\n"; return 1; }

    //Decifra
    try {
      descriptografa(arquivo_cifrado, arquivo_claro, cubo, padding);
      
      //Fecha arquivos
      arquivo_claro.close();
      arquivo_cifrado.close();
      
      //Apaga o byte do padding caso exista
      if(padding){
        uintmax_t tamanho_atual = std::filesystem::file_size(claro_nome);
        if (tamanho_atual > 0) 
          filesystem::resize_file(claro_nome, tamanho_atual - 1);
      }
    }catch (const std::filesystem::filesystem_error& e) {
        cerr << "Erro ao descriptografar: " << e.what() << std::endl;
    }

    //Imprime saida
    cout<<"'"<<cifrado_nome<<"' decifrado em '"<<claro_nome<<"' com sucesso.\n";
    string comando_cksum="cksum "+claro_nome;
    system(comando_cksum.data());
    
    break;
  }
  case 3:{ //gera chave
    chave_estrutura=gera_chave();
        
    //Abre arquivo para escrita
    ofstream arquivo_chave(chave_nome, ios::binary);
    if (!arquivo_chave) {cerr << "Erro ao abrir o arquivo para escrita.\n"; return 1;}

    try{salvar_chave(arquivo_chave, *(chave_estrutura));} 
    catch (const runtime_error& e) {
      cerr << "Excecao ao salvar: " << e.what() << endl;
      return 1;
    }
    
    arquivo_chave.close();
    cout<<"Chave salva em '"<<chave_nome<<"' com sucesso.\n";
    string comando_cksum="cksum "+chave_nome;
    system(comando_cksum.data());
    
    destroi_chave(chave_estrutura);
    break;
  }
  default: //Parametros da main errados
    return 1; 
    break;
  }

  destroi_cubo(cubo);
  
  return 0;
}


/*
  Processa argc e argv, imprime o --help.
  Se os parametros estiverem corretos preenche as strings com o nome dos arquivos que serao usados nas opicoes
  Retorna em caso de erro, 1 para cifrar e 2 para decifrar
*/
int processa_arg(int argc, char** argv, string& cifrado, string& claro, string& chave){
  
  //Printa o help
  if(argc == 2 && !strcmp(argv[1],"--help")){
    cout<<"Esse programa recebe algums parametros na linha de comando:\n";
    cout<<"  -d : indica que o programa vai decriptar, espera -e <param> -s <param> e -k <param>\n";
    cout<<"\n  -c : indica que o programa vai cifrar , espera -e <param> -s <param> e -k <param>\n";
    cout<<"\n  -g : indica que o programa vai gerar um arquivo com o uma chave aleatoria, espera -s <param>\n";
    cout<<"\n Onde <param> sao os nomes dos arquivos de texto.\n";
    cout<<"\nOpcoes complementares:\n";
    cout<<"\n  -e : indica o arquivo de entrada.\n";
    cout<<"\n  -s : indica o arquivo de saida. Se existir sera sobreescrito, se nao existir cria um no diretorio corrente. \n";
    cout<<"\n  -k : indica que o proximo parametro contem o nome do arquivo com a chave que sera usada na cifra, gera erro se o arquivo nao existir ou se a chave nao estiver no formato esperado.\n";
    cout<<"\nExemplos:\n ./cripto -c -e in -s out -k chave\n ./cripto -d -e cifrado -s saida -k chave\n";
    return 0;
  }

  
  //Conta os parametros
  if(argc != 8 && argc != 4 ){ 
    cerr<<"Paramentros incorretos, --help para mais informacoes.\n";
    return 0;
  }
  
  //Flags para validar a entrada
  int opt, tipo = getopt(argc, argv, "cdg" );
  bool fe{1}, fs{1}, fk{1};

  switch (tipo)
  {
  case 'c':
    while ((opt = getopt (argc, argv, "e:s:k:")) != -1)
      switch (opt)
        {
        case 'e': 
          claro = optarg;
          fe=0;
          break;
        case 's':
          cifrado = optarg;
          fs=0;
          break;
        case 'k':
          chave = optarg;
          fk=0;
          break;
        default:
          cerr<<"Opcao nao identificada '"<<opterr<<"' encontrado.\n";
        }
    break;
  case 'd':
    while ((opt = getopt (argc, argv, "e:s:k:")) != -1)
      switch (opt)
        {
        case 'e':
          cifrado = optarg;
          fe=0;
          break;
        case 's':
          claro = optarg;
          fs=0;
          break;
        case 'k':
          chave = optarg;
          fk=0;
          break;
        default:
          cerr<<"Opcao nao identificada '"<<opterr<<"' encontrado.\n";
        }    
    break;
    case 'g':
      while ((opt = getopt (argc, argv, "s:")) != -1)
        switch (opt)
          {
          case 's':
            chave = optarg;
            fk=0;
            break;
          default:
            cerr<<"Opcao nao identificada '"<<opterr<<"' encontrado.\n";
          }    
      break;
  
  default:
    cerr<<"Opcao nao identificada '"<<opterr<<"' encontrado.\n";
    break;
  }
  if((fe || fs || fk ) && !(tipo == 'g' || fk)){
    cerr<<"Paramentros incorretos, --help para mais informacoes.\n";
    return 0;
  }

  if(tipo == 'c') return 1; //Cifrar
  if(tipo == 'd') return 2; //Decifrar
  if(tipo == 'g') return 3; //Gera chave

  return 0;
}

