#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

#define MAXIMO_MUTACOES 3    //4
#define DIMENSAO_ENTRADA 4    //palavra de entrada pro circuito
#define TAM_TAB_VERDADE (int)pow(2,DIMENSAO_ENTRADA)
#define NUMERO_MAX_GERACOES 1000000000 //1.000.000.000
#define INDIVIDUOS_POR_GERACAO 4    //5 
#define NUMERO_PORTAS 5
#define AND 100
#define OR 200
#define NAND 300
#define NOR 400
#define XOR 500
#define LINHAS 4
#define COLUNAS 4
#define NUMERO_NOS DIMENSAO_ENTRADA + LINHAS * COLUNAS

/*
 * Pensei em mutações seletivas: Mutar apenas portas, mutar apenas topologia, mutar ambos. Trabalhos futuros.
 * 
 */

int geraInt(int limiteSuperior){
    return(rand()%limiteSuperior);
}

int sorteiaPorta(){
    int porta = geraInt(5)+1;
    switch(porta){
        case 1:
            return AND;
            break;
        case 2:
            return OR;
            break;
        case 3:
            return NAND;
            break;
        case 4:
            return NOR;
            break;
        case 5:
            return XOR;
            break;
    }
}

void decimalParaBinario(int bin[DIMENSAO_ENTRADA], int dec, int tam) {
    int j=0, i;
	int temp[tam];
	while( dec != 0){
		temp[j++] = dec%2;
		dec/=2;
	}
	for (; j < tam ; j++ )
        temp[j] = 0;
	for ( i = 0 ; i < tam ; i++ )
		bin[i] = temp[tam-i-1];
}

void inicializaEntrada(int e[TAM_TAB_VERDADE][DIMENSAO_ENTRADA]){
    int i, valor = 0;
    for ( i = 0 ; i < TAM_TAB_VERDADE ; i++ )
        decimalParaBinario(e[i],valor++,DIMENSAO_ENTRADA);
}

void inicializaSaida(int s[TAM_TAB_VERDADE]){
    int i;
    for ( i = 0 ; i < TAM_TAB_VERDADE ; i++ ){
        s[i] = geraInt(2);
    }
}

void imprimeTabelaVerdade(int e[TAM_TAB_VERDADE][DIMENSAO_ENTRADA], int s[TAM_TAB_VERDADE]){
    int i, j;
    printf("\n\n");
    for ( i = 0 ; i < TAM_TAB_VERDADE ; i++ ){
        for ( j = 0 ; j < DIMENSAO_ENTRADA ; j++ ){
            printf("%d ",e[i][j]);
        }
        printf("%d \n",s[i]);
    }
    printf("\n\n");
}

void geraIndividuoInicial(int ind[3*NUMERO_NOS+3]){
    int i;
    ind[0] = 1;
    int colunaAtualNaTopologia = 0, linhaAtualNaTopologia = 0;
    for ( i = 1 ; i <= 3*(NUMERO_NOS-DIMENSAO_ENTRADA) ; i = i + 3 ){
        ind[i] = ind[i+1] = geraInt((DIMENSAO_ENTRADA + colunaAtualNaTopologia * LINHAS)) + 1;
        while (ind[i] == ind[i+1]){    //
            ind[i+1] = geraInt((DIMENSAO_ENTRADA + colunaAtualNaTopologia * LINHAS)) + 1;
        }
        ind[i+2] = sorteiaPorta();
        if (linhaAtualNaTopologia++ == LINHAS-1){
            linhaAtualNaTopologia = 0;
            colunaAtualNaTopologia++;
        }
    }
    ind[i++] = geraInt(NUMERO_NOS)+1;
    ind[i] = -1; //A ser calculado
}

char * getNomePorta(int p){
    switch(p){
        case 100:
            return "AND";
            break;
        case 200:
            return "OR";
            break;
        case 300:
            return "NAND";
            break;
        case 400:
            return "NOR";
            break;
        case 500:
            return "XOR";
            break;
    }
}


void imprimeIndividuo(int ind[3*(NUMERO_NOS-DIMENSAO_ENTRADA)+3]){
    int i;
    //printf("\nIndividuo\n");
    printf("\n");
    for ( i = 0 ; i < 3*(NUMERO_NOS-DIMENSAO_ENTRADA)+3 ; i++ ){
        if ( i % 3 == 1 ) { //Terminou uma trinca (ou um nó)
            printf("| ");
        }
        if ( i % 3 == 0 && i != 0 ){   //É porta lógica
            printf("%s ",getNomePorta(ind[i]));
        }
        else{
            printf("%d ",ind[i]);
        }
    }
    printf("\n");
}

void geraMatrizTopologia(int m[LINHAS][COLUNAS]){
    //padrão: de cima pra baixo e depois vai andando pra direita
    int i, j, no = DIMENSAO_ENTRADA + 1;
    for ( i = 0 ; i < COLUNAS; i++ ){
        for ( j = 0 ; j < LINHAS ; j++ ){
            m[j][i] = no++;
        }
    }
}

void imprimeMatriz(int m[LINHAS][COLUNAS], int linhas, int colunas){
    int i, j;
    for ( i = 0 ; i < linhas; i++ ){
        for ( j = 0 ; j < colunas ; j++ ){
            printf("%d ",m[i][j]);
        }
        printf("\n");
    }
}

int resolvePorta(int eUm, int eDois, int porta){
    switch(porta){
        case 100:
            return eUm & eDois;
            break;
        case 200:
            return eUm | eDois;
            break;
        case 300:
            return !(eUm & eDois);
            break;
        case 400:
            return !(eUm | eDois);
            break;
        case 500:
            return eUm ^ eDois;
            break;
    }
}

int calculaSaida(int ind[3*(NUMERO_NOS-DIMENSAO_ENTRADA)+3], int e[DIMENSAO_ENTRADA]){
    int topologiaAtual[LINHAS][COLUNAS];
    int i, j, k = 1, a, b;    // k = posicaoNoIndividuo
    int pos = DIMENSAO_ENTRADA + 1;    //posição na matrizTopologia  
    int nohAtual;
    if ( ind[3*(NUMERO_NOS-DIMENSAO_ENTRADA)+3-1-1] <= DIMENSAO_ENTRADA ){
        return e[ind[3*(NUMERO_NOS-DIMENSAO_ENTRADA)+3-1-1]-1];
    }
    for ( j = 0 ; j < COLUNAS; j++ ){
        for ( i = 0 ; i < LINHAS ; i++ ){
            //printf("\n Entrada: %d %d %d %d. Trinca em análise: %d %d %s",e[0],e[1],e[2],e[3],ind[k],ind[k+1],getNomePorta(ind[k+2]));
            if ( ind[k] <= DIMENSAO_ENTRADA ){  //entrada um é porta de entrada
                if ( ind[k+1] <= DIMENSAO_ENTRADA ){    //entrada dois é porta de entrada
                    topologiaAtual[i][j] = resolvePorta(e[ind[k]-1], e[ind[k+1]-1], ind[k+2]);
                    //printf("\tResultado: %d (ponto 1)",topologiaAtual[i][j]);
                }
                else{   //entrada dois não é porta de entrada (tem que pegar o estado atual na topologiaAtual)
                    //Achando o estado atual na topologiaAtual
                    nohAtual = DIMENSAO_ENTRADA + 1;
                    for ( b = 0 ; b < COLUNAS; b++ ){
                        for ( a = 0 ; a < LINHAS ; a++ ){
                            if ( nohAtual++ == ind[k+1] ){
                                topologiaAtual[i][j] = resolvePorta(e[ind[k]-1], topologiaAtual[a][b], ind[k+2]);
                                //printf("\tResultado: %d (ponto 2)",topologiaAtual[i][j]);
                                b = COLUNAS;
                                break;
                            }
                        }
                    }
                }
            }
            else{   // entrada um não é porta de entrada (tem que pegar o estado atual na topologiaAtual)
                if ( ind[k+1] <= DIMENSAO_ENTRADA ){    // entrada dois é porta de entrada
                    //Achando o estado atual na topologiaAtual
                    nohAtual = DIMENSAO_ENTRADA + 1;
                    for ( b = 0 ; b < COLUNAS; b++ ){
                        for ( a = 0 ; a < LINHAS ; a++ ){
                            if ( nohAtual++ == ind[k] ){
                                topologiaAtual[i][j] = resolvePorta(topologiaAtual[a][b], e[ind[k+1]-1], ind[k+2]);
                                //printf("\tResultado: %d (ponto 3)",topologiaAtual[i][j]);
                                b = COLUNAS;
                                break;
                            }
                        }
                    }
                }
                else{   // entrada dois não é porta de entrada (tem que pegar o estado atual na topologiaAtual)
                    //Achando o estado atual na topologiaAtual
                    int auxEntrUm, auxEntrDois;
                    nohAtual = DIMENSAO_ENTRADA + 1;
                    for ( b = 0 ; b < COLUNAS; b++ ){
                        for ( a = 0 ; a < LINHAS ; a++ ){
                            if ( nohAtual++ == ind[k] ){
                                auxEntrUm = topologiaAtual[a][b];
                                b = COLUNAS;
                                break;
                            }
                        }
                    }
                    nohAtual = DIMENSAO_ENTRADA + 1;
                    for ( b = 0 ; b < COLUNAS; b++ ){
                        for ( a = 0 ; a < LINHAS ; a++ ){
                            if ( nohAtual++ == ind[k+1] ){
                                auxEntrDois = topologiaAtual[a][b];
                                b = COLUNAS;
                                break;
                            }
                        }
                    }
                    topologiaAtual[i][j] = resolvePorta(auxEntrUm, auxEntrDois, ind[k+2]);
                    //printf("\tResultado: %d (ponto 4)",topologiaAtual[i][j]);
                }
            }
            if ( pos++ == ind[3*(NUMERO_NOS-DIMENSAO_ENTRADA)+3-1-1] ){  //pos = posicao da saída no indivíduo
                return(topologiaAtual[i][j]);
            }
            k+=3;
        }
    }
}

int avaliaIndividuo(int ind[3*(NUMERO_NOS-DIMENSAO_ENTRADA)+3], int e[TAM_TAB_VERDADE][DIMENSAO_ENTRADA], int s[TAM_TAB_VERDADE]){
    int rank = 0, i;
    for ( i = 0 ; i < TAM_TAB_VERDADE ; i++ ){
        if ( calculaSaida( ind, e[i] ) == s[i] ){
            rank++;
        }
    }
    return rank;
}

int selecionaMelhorIndividuoDaGeracao(int g[INDIVIDUOS_POR_GERACAO][3*(NUMERO_NOS-DIMENSAO_ENTRADA)+3]){
    int maiorRank = -1, i, posicao = -1;
    for ( i = 0 ; i < INDIVIDUOS_POR_GERACAO ; i++ ){
        if ( g[i][3*(NUMERO_NOS-DIMENSAO_ENTRADA)+3-1] > maiorRank ){
            maiorRank = g[i][3*(NUMERO_NOS-DIMENSAO_ENTRADA)+3-1];
            posicao = i;
        }
    }
    return(posicao);
}

void modificaIndividuo(int ind[3*(NUMERO_NOS-DIMENSAO_ENTRADA)+3]){
    int i, j, posicaoAMudar, portaTemp;
    ind[0] = 1; //Sempre 1. Pode ser incrementado em usos futuros (para persistência dos individuos)
    int colunaAtualNaTopologia = 0, linhaAtualNaTopologia = 0;
    for ( j = 1 ; j <= MAXIMO_MUTACOES ; j++ ){
        colunaAtualNaTopologia = 0; //Reseta a coluna da topologia para a nova mutação
        posicaoAMudar = geraInt(3*(NUMERO_NOS-DIMENSAO_ENTRADA)+3-2) + 1;
        for ( i = 1 ; i <= 3*(NUMERO_NOS-DIMENSAO_ENTRADA) ; i = i + 3 ){
            if ( i == posicaoAMudar ){
                portaTemp = geraInt((DIMENSAO_ENTRADA + colunaAtualNaTopologia * LINHAS)) + 1;
                while (portaTemp == ind[i+1]){
                    portaTemp = geraInt((DIMENSAO_ENTRADA + colunaAtualNaTopologia * LINHAS)) + 1;
                }
                ind[i] = portaTemp;
                i = 3*(NUMERO_NOS-DIMENSAO_ENTRADA)+1;
                break;
            }
            if ( i+1 == posicaoAMudar ){
                ind[i+1] = ind[i];
                while (ind[i] == ind[i+1]){
                    ind[i+1] = geraInt((DIMENSAO_ENTRADA + colunaAtualNaTopologia * LINHAS)) + 1;
                }
                i = 3*(NUMERO_NOS-DIMENSAO_ENTRADA)+1;
                break;
            }
            if ( i+2 == posicaoAMudar ){
                ind[i+2] = sorteiaPorta();
                i = 3*(NUMERO_NOS-DIMENSAO_ENTRADA)+1;
                break;
            }
            if (linhaAtualNaTopologia++ == LINHAS-1){
                linhaAtualNaTopologia = 0;
                colunaAtualNaTopologia++;
            }
        }
        if ( i == posicaoAMudar ){  //saída
            ind[i] = geraInt(NUMERO_NOS)+1;
        }
        //Deixei o rank anterior (última posição) inalterado, pois em algum momento futuro ela será recalculado
    }
}


int main(){
    
    srand(time(NULL));
    
    int saida[TAM_TAB_VERDADE];
    int individuo[3*(NUMERO_NOS-DIMENSAO_ENTRADA)+3];  //ID,NO,NO,PORTA,...,NO,NO,PORTA,SAIDA,PONTUACAO
    int geracao[INDIVIDUOS_POR_GERACAO][3*(NUMERO_NOS-DIMENSAO_ENTRADA)+3];
    int entrada[TAM_TAB_VERDADE][DIMENSAO_ENTRADA];   //Em cada linha: posição 0 = entrada 1; posicao 1 = entrada 2; etc...
    int matrizTopologia[LINHAS][COLUNAS];
    int numeroGeracao = 1, melhorRank = 0, melhorIndividuo, melhorRankAteOMomento = -1;
    int i, j;
    
    printf("\n\n Número de nós: %d\n",NUMERO_NOS);
    
    inicializaEntrada(entrada);
    inicializaSaida(saida);
    imprimeTabelaVerdade(entrada,saida);
    
    geraMatrizTopologia(matrizTopologia);
    imprimeMatriz(matrizTopologia,LINHAS,COLUNAS);
    
    geraIndividuoInicial(individuo);
    
    //imprimeIndividuo(individuo);
    
    individuo[3*(NUMERO_NOS-DIMENSAO_ENTRADA)+3-1] = avaliaIndividuo(individuo, entrada, saida);
    
//    printf("\nIndivíduo primordial: ");
//    imprimeIndividuo(individuo);
        
    //Preenche toda a primeira geração com o indivíduo inicial
    for ( i = 0 ;  i < INDIVIDUOS_POR_GERACAO ; i++ ){
        for ( j = 0 ; j < 3*(NUMERO_NOS-DIMENSAO_ENTRADA)+3 ; j++ ){
            geracao[i][j] = individuo[j];
        }
    }

    /*
    for ( i = 0 ;  i < INDIVIDUOS_POR_GERACAO ; i++ ){
        imprimeIndividuo(geracao[i]);
    }*/
    
    while ( numeroGeracao <= NUMERO_MAX_GERACOES && melhorRank < TAM_TAB_VERDADE ){
        melhorIndividuo = selecionaMelhorIndividuoDaGeracao(geracao);
        //printf("\nPosição do melhor: %d",melhorIndividuo);
        melhorRank = geracao[melhorIndividuo][3*(NUMERO_NOS-DIMENSAO_ENTRADA)+3-1];
        //imprimeIndividuo(geracao[melhorIndividuo]);
        //printf("\nMelhor rank: %d\n",melhorRank);
        
        
        /*//Replica o melhor por toda a geração
        for ( i = 0 ; i < INDIVIDUOS_POR_GERACAO ; i++ ){
			for ( j = 0 ; j < 3*(NUMERO_NOS-DIMENSAO_ENTRADA)+3 ; j++ ){
            	geracao[i][j] = geracao[melhorIndividuo][j];
        	}
		}*/
        
        //Coloca o melhor no topo da geração (deixa a cópia dele na posição original) e não muda os demais.
        for ( j = 0 ; j < 3*(NUMERO_NOS-DIMENSAO_ENTRADA)+3 ; j++ ){
            geracao[0][j] = geracao[melhorIndividuo][j];
        }
        
        //Introduz mutações nos outros (n-1) indivíduos
        for ( i = 1 ;  i < INDIVIDUOS_POR_GERACAO ; i++ ){
            modificaIndividuo(geracao[i]);
            geracao[i][3*(NUMERO_NOS-DIMENSAO_ENTRADA)+3-1] = avaliaIndividuo(geracao[i], entrada, saida);
        }
        
        /*for ( i = 0 ;  i < INDIVIDUOS_POR_GERACAO ; i++ ){
            imprimeIndividuo(geracao[i]);
        }*/
        
        if (melhorRank > melhorRankAteOMomento){
            printf("\n EVOLUÇÃO!!!!  Geração: %d. \t Melhor rank até esta geração: %d",numeroGeracao,melhorRank);
            imprimeIndividuo(geracao[0]);
            melhorRankAteOMomento = melhorRank;
        }
        numeroGeracao++;
    }
    if (numeroGeracao == NUMERO_MAX_GERACOES+1){
        printf("\n\n Solução não encontrada.");
    }
    else{
        printf("\n\nSolução encontrada na geração %d: ",numeroGeracao-1);
        imprimeIndividuo(geracao[0]);
    }
    printf("\n\n");
}
 
