#define _CRT_SECURE_NO_WARNINGS
#define CAMINHO_IMAGENS "imagens/"
#define CAMINHO_SONS    "sons/"


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

// ==========================================//
// --- 1. DEFINIÇÕES GERAIS E CONSTANTES --- //
// ==========================================//

const int LARGURA_TELA = 800;
const int ALTURA_TELA = 600;
const float FPS = 60.0;

const float VELOCIDADE_CENARIO = 7.0f;
const int DINO_TELA_W = 100;
const int DINO_TELA_H = 175;
const int MAMAE_W = 160;
const int MAMAE_H = 230;
const int LINHA_DO_CHAO = 455;
#define ALTURA_CHAO (LINHA_DO_CHAO - DINO_TELA_H + 5)
#define ALTURA_CHAO_MAMAE (LINHA_DO_CHAO - MAMAE_H + 25) 
const float GRAVIDADE = 0.8f;
const float VELOCIDADE_PULO = -15.0f;

#define MAX_OBSTACULOS 5
const int OBS_W = 110;
const int OBS_H = 110;
#define QTD_TIPOS_OBS 5
const int ITEM_W = 50;
const int ITEM_H = 50;

enum {
	IMG_DINO_CHAO = 0,
	IMG_DINO_SUBINDO,
	IMG_DINO_TOPO,
	IMG_DINO_CAINDO,
	NUM_IMAGENS_DINO
};

typedef enum {
	ESTADO_MENU,
	ESTADO_SELECAO,
	ESTADO_HISTORIA,
	ESTADO_EM_JOGO,
	ESTADO_GAMEOVER,
	ESTADO_CUTSCENE,
	ESTADO_OPCOES,
	ESTADO_VITORIA
} GameState;

typedef enum {
	PERSONAGEM_NENHUM,
	PERSONAGEM_SKATISTA,
	PERSONAGEM_PATINADORA
} Personagem;

// =============================== //
// --- 2. ESTRUTURAS (STRUCTS) --- //
// =============================== //

typedef struct {
	float x, y;
	float largura, altura;
} Botao;

typedef struct {
	float x, y, vel_y;
	bool esta_pulando;
	ALLEGRO_BITMAP* sprites[NUM_IMAGENS_DINO];
} Dino;

typedef struct {
	float x, y;
	bool ativo;
	int tipo_imagem;
} Obstaculo;

typedef struct {
	float x, y;
	bool ativo;
	bool coletado;
	int tipo;
} Item;

typedef struct {
	float x, y;
	int frame_atual;
	int contador_animacao;
	ALLEGRO_BITMAP* sprites[3];
} Mamae;

// ============================ //
// --- 3. VARIÁVEIS GLOBAIS --- //
// ============================ //

ALLEGRO_DISPLAY* janela = NULL;
ALLEGRO_EVENT_QUEUE* fila_eventos = NULL;
ALLEGRO_TIMER* timer = NULL;
ALLEGRO_FONT* fonte_debug = NULL;

ALLEGRO_BITMAP* fundo_menu_bitmap = NULL;
ALLEGRO_BITMAP* tela_selecao_bitmap = NULL;
ALLEGRO_BITMAP* historia_dudu_bitmap = NULL;
ALLEGRO_BITMAP* historia_nina_bitmap = NULL;
ALLEGRO_BITMAP* img_gameover_nina = NULL;
ALLEGRO_BITMAP* img_gameover_dudu = NULL;
ALLEGRO_BITMAP* img_vitoria = NULL;

ALLEGRO_BITMAP* op_sound_music = NULL;
ALLEGRO_BITMAP* op_sound_nomusic = NULL;
ALLEGRO_BITMAP* op_nosound_music = NULL;
ALLEGRO_BITMAP* op_nosound_nomusic = NULL;

ALLEGRO_BITMAP* fundo_jogo[3] = { NULL, NULL, NULL };
ALLEGRO_BITMAP* img_obstaculos[QTD_TIPOS_OBS];
ALLEGRO_BITMAP* img_batom = NULL;
ALLEGRO_BITMAP* img_esmalte = NULL;
ALLEGRO_BITMAP* img_coracao = NULL;

Dino dino;
Mamae mamae;
Obstaculo obstaculos[MAX_OBSTACULOS];
Item itens[2];

GameState estado_atual = ESTADO_MENU;
Personagem personagem_selecionado = PERSONAGEM_NENHUM;

float mouse_x = 0;
float mouse_y = 0;
bool mouse_botao_pressionado = false;
bool rodando = true;
bool desenhar = true;

bool jogo_com_movimento = false;
float posicao_x_cenario = 0;
int largura_fundo = 0;
int frame_atual_dino = IMG_DINO_CHAO;
int contador_spawn = 0;
int tempo_para_proximo = 0;
long frames_decorridos = 0;
int itens_coletados = 0;
int voltas_do_cenario = 0;

bool cutscene_iniciada = false;
bool cutscene_andando = false;
bool encontro_aconteceu = false;
bool som_misterio_iniciado = false;

bool som_ativado = true;
bool musica_ativada = true;
bool musica_menu_tocando = false;
bool musica_selecao_tocando = false;
bool musica_opcoes_tocando = false;

Botao botao_start, botao_options, botao_exit;
Botao botao_continue_selecao, botao_return_menu;
Botao area_dino_skate, area_dino_patins;
Botao botao_yes_i_do, botao_no_i_dont, botao_return_historia;
Botao area_click_sfx_on, area_click_sfx_off;
Botao area_click_music_on, area_click_music_off;
Botao botao_confirmar, botao_retornar_menu;

Botao botao_go_tryagain;
Botao botao_go_menu;
Botao botao_vitoria_playagain;
Botao botao_vitoria_returnmenu;
Botao botao_vitoria_nextlevel;


ALLEGRO_SAMPLE* som_mouse_click = NULL;
ALLEGRO_SAMPLE* som_fala_dino = NULL;
ALLEGRO_AUDIO_STREAM* musica_menu_inicial = NULL;
ALLEGRO_AUDIO_STREAM* musica_selecao = NULL;
ALLEGRO_AUDIO_STREAM* musica_opcoes = NULL;
ALLEGRO_SAMPLE_ID id_fala_dino;
bool fala_dino_iniciada = false;

ALLEGRO_SAMPLE* som_pulo = NULL;
ALLEGRO_SAMPLE* som_colisao = NULL;
ALLEGRO_SAMPLE_ID id_som_colisao;
ALLEGRO_AUDIO_STREAM* musica_jogo = NULL;
ALLEGRO_SAMPLE* musica_gameover = NULL;
ALLEGRO_SAMPLE* musica_vitoria = NULL;
bool musica_jogo_tocando = false;
bool musica_gameover_tocando = false;
bool musica_vitoria_tocando = false;

ALLEGRO_SAMPLE_ID id_musica_vitoria;

ALLEGRO_SAMPLE* som_item_coletado = NULL;
ALLEGRO_SAMPLE* som_encontro = NULL;
ALLEGRO_SAMPLE* som_misterio = NULL;


// ================================ // 
// --- 4. PROTÓTIPOS DE FUNÇÕES --- //
// ================================ //

void carregar_recursos_jogo();
void resetar_jogo();
void configurar_sprites_personagem();
void atualizar_jogo_geral();
void atualizar_cutscene();
void desenhar_jogo();
void desenhar_cutscene();
void desenhar_gameover();
void desenhar_vitoria();
bool verificar_colisao_obs(Dino* d, Obstaculo* obs);
bool verificar_colisao_item(Dino* d, Item* it);

void tocar_clique();
void tocar_som_evento(ALLEGRO_SAMPLE* som);
void parar_toda_musica();
void gerenciar_musica_jogo(GameState novo_estado);
void gerenciar_musica_vitoria(GameState novo_estado);

void gerenciar_musica_menu(GameState novo_estado);
void gerenciar_musica_selecao(GameState novo_estado);
void gerenciar_musica_opcoes(GameState novo_estado);
bool mouse_sobre_botao(Botao* b, float mx, float my);

// =============================================== //
// --- 5. IMPLEMENTAÇÃO DAS FUNÇÕES AUXILIARES --- //
// =============================================== //

bool mouse_sobre_botao(Botao* b, float mx, float my) {
	return (mx >= b->x && mx <= b->x + b->largura &&
		my >= b->y && my <= b->y + b->altura);
}

void tocar_clique() {
	if (som_mouse_click && som_ativado) {
		al_play_sample(som_mouse_click, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	}
}

void tocar_som_evento(ALLEGRO_SAMPLE* som) {
	if (som && som_ativado) {
		al_play_sample(som, 1.5, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	}
}

void parar_toda_musica() {
	if (musica_menu_inicial && musica_menu_tocando) {
		al_set_audio_stream_playing(musica_menu_inicial, false);
		al_detach_audio_stream(musica_menu_inicial);
		musica_menu_tocando = false;
	}
	if (musica_selecao && musica_selecao_tocando) {
		al_set_audio_stream_playing(musica_selecao, false);
		al_detach_audio_stream(musica_selecao);
		musica_selecao_tocando = false;
	}
	if (musica_opcoes && musica_opcoes_tocando) {
		al_set_audio_stream_playing(musica_opcoes, false);
		al_detach_audio_stream(musica_opcoes);
		musica_opcoes_tocando = false;
	}
	if (musica_jogo && musica_jogo_tocando) {
		al_set_audio_stream_playing(musica_jogo, false);
		al_detach_audio_stream(musica_jogo);
		musica_jogo_tocando = false;
	}
	if (musica_vitoria_tocando) {
		al_stop_sample(&id_musica_vitoria);
		musica_vitoria_tocando = false;
	}
}

void gerenciar_musica_menu(GameState novo_estado) {
	if (novo_estado == ESTADO_MENU) {
		if (musica_menu_inicial && !musica_menu_tocando && musica_ativada) {
			parar_toda_musica();
			al_set_audio_stream_gain(musica_menu_inicial, 1.0);
			al_set_audio_stream_playmode(musica_menu_inicial, ALLEGRO_PLAYMODE_LOOP);
			al_attach_audio_stream_to_mixer(musica_menu_inicial, al_get_default_mixer());
			al_set_audio_stream_playing(musica_menu_inicial, true);
			musica_menu_tocando = true;
		}
	}
	else {
		if (musica_menu_inicial && musica_menu_tocando) {
			al_set_audio_stream_playing(musica_menu_inicial, false);
			al_detach_audio_stream(musica_menu_inicial);
			musica_menu_tocando = false;
		}
	}
}

void gerenciar_musica_selecao(GameState novo_estado) {
	if (novo_estado == ESTADO_SELECAO || novo_estado == ESTADO_HISTORIA) {
		if (musica_selecao && !musica_selecao_tocando && musica_ativada) {
			parar_toda_musica();
			al_set_audio_stream_gain(musica_selecao, 1.0);
			al_set_audio_stream_playmode(musica_selecao, ALLEGRO_PLAYMODE_LOOP);
			al_attach_audio_stream_to_mixer(musica_selecao, al_get_default_mixer());
			al_set_audio_stream_playing(musica_selecao, true);
			musica_selecao_tocando = true;
		}
	}
	else {
		if (musica_selecao && musica_selecao_tocando) {
			al_set_audio_stream_playing(musica_selecao, false);
			al_detach_audio_stream(musica_selecao);
			musica_selecao_tocando = false;
		}
	}
}

void gerenciar_musica_opcoes(GameState novo_estado) {
	if (novo_estado == ESTADO_OPCOES) {
		if (musica_opcoes && !musica_opcoes_tocando && musica_ativada) {
			parar_toda_musica();
			al_set_audio_stream_gain(musica_opcoes, 0.4);
			al_set_audio_stream_playmode(musica_opcoes, ALLEGRO_PLAYMODE_LOOP);
			al_attach_audio_stream_to_mixer(musica_opcoes, al_get_default_mixer());
			al_set_audio_stream_playing(musica_opcoes, true);
			musica_opcoes_tocando = true;
		}
	}
	else {
		if (musica_opcoes && musica_opcoes_tocando) {
			al_set_audio_stream_playing(musica_opcoes, false);
			al_detach_audio_stream(musica_opcoes);
			musica_opcoes_tocando = false;
		}
	}
}

void gerenciar_musica_jogo(GameState novo_estado) {
	if (novo_estado == ESTADO_EM_JOGO) {
		if (musica_jogo && !musica_jogo_tocando && musica_ativada) {
			parar_toda_musica();
			al_rewind_audio_stream(musica_jogo);
			al_set_audio_stream_gain(musica_jogo, 0.4);
			al_set_audio_stream_playmode(musica_jogo, ALLEGRO_PLAYMODE_LOOP);
			al_attach_audio_stream_to_mixer(musica_jogo, al_get_default_mixer());
			al_set_audio_stream_playing(musica_jogo, true);
			musica_jogo_tocando = true;
		}
	}
	else {
		if (musica_jogo && musica_jogo_tocando) {
			al_set_audio_stream_playing(musica_jogo, false);
			al_detach_audio_stream(musica_jogo);
			musica_jogo_tocando = false;
		}
	}
}

void gerenciar_musica_vitoria(GameState novo_estado) {
	if (novo_estado == ESTADO_VITORIA) {
		if (musica_vitoria && !musica_vitoria_tocando && musica_ativada) {
			parar_toda_musica();
			al_play_sample(musica_vitoria, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &id_musica_vitoria);
			musica_vitoria_tocando = true;
		}
	}
	else {
		if (musica_vitoria_tocando) {
			al_stop_sample(&id_musica_vitoria);
		}
		musica_vitoria_tocando = false;
	}
}


// ==================================== //
// --- 6. FUNÇÕES DE LÓGICA DO JOGO --- //
// ==================================== //

void carregar_recursos_jogo() {
	fundo_jogo[0] = al_load_bitmap(CAMINHO_IMAGENS "fundo_1.png");
	fundo_jogo[1] = al_load_bitmap(CAMINHO_IMAGENS "fundo_2.png");
	fundo_jogo[2] = al_load_bitmap(CAMINHO_IMAGENS "fundo_3.png");
	if (fundo_jogo[0]) largura_fundo = al_get_bitmap_width(fundo_jogo[0]);

	img_batom = al_load_bitmap(CAMINHO_IMAGENS "batom.png");
	img_esmalte = al_load_bitmap(CAMINHO_IMAGENS "esmalte.png");
	img_gameover_nina = al_load_bitmap(CAMINHO_IMAGENS "gameover_nina.png");
	img_gameover_dudu = al_load_bitmap(CAMINHO_IMAGENS "gameover_dudu.png");

	img_vitoria = al_load_bitmap(CAMINHO_IMAGENS "menu_win.png");
	img_coracao = al_load_bitmap(CAMINHO_IMAGENS "coracao.png");
	if (!img_coracao) {
		img_coracao = al_load_bitmap(CAMINHO_IMAGENS "coracao.jpg");
	}

	som_item_coletado = al_load_sample(CAMINHO_SONS "som_coleta_item.mp3");
	if (!som_item_coletado) {
		fprintf(stderr, "AVISO: Falha ao carregar o som 'som_coleta_item.mp3'. O som de coleta não funcionará.\n");
	}

	som_encontro = al_load_sample(CAMINHO_SONS "som_encontro.mp3");
	if (!som_encontro) {
		fprintf(stderr, "AVISO: Falha ao carregar o som 'som_encontro.mp3'.\n");
	}

	som_misterio = al_load_sample(CAMINHO_SONS "som_misterio.mp3");
	if (!som_misterio) {
		fprintf(stderr, "AVISO: Falha ao carregar o som 'som_misterio.mp3'.\n");
	}


	if (!img_vitoria) {
		img_vitoria = al_load_bitmap(CAMINHO_IMAGENS "menu_win.jpg");
		if (!img_vitoria) {
			fprintf(stderr, "ERRO: Falha ao carregar a imagem de vitória.\n");
		}
	}
	if (!img_coracao) {
		fprintf(stderr, "AVISO: Falha ao carregar a imagem do coracao. O fallback primitivo será usado.\n");
	}


	char nomes_obs[QTD_TIPOS_OBS][30] = { CAMINHO_IMAGENS "obstaculo_1.png", CAMINHO_IMAGENS "obstaculo_2.png", CAMINHO_IMAGENS "obstaculo_3.png", CAMINHO_IMAGENS "tronco_1.png", CAMINHO_IMAGENS "tronco_2.png" };
	for (int i = 0; i < QTD_TIPOS_OBS; i++) img_obstaculos[i] = al_load_bitmap(nomes_obs[i]);

	mamae.sprites[0] = al_load_bitmap(CAMINHO_IMAGENS "mamae_1.png");
	mamae.sprites[1] = al_load_bitmap(CAMINHO_IMAGENS "mamae_2.png");
	mamae.sprites[2] = al_load_bitmap(CAMINHO_IMAGENS "mamae_3.png");

	op_sound_music = al_load_bitmap(CAMINHO_IMAGENS "sound_music.png");
	op_sound_nomusic = al_load_bitmap(CAMINHO_IMAGENS "sound_nomusic.png");
	op_nosound_music = al_load_bitmap(CAMINHO_IMAGENS "nosound_music.png");
	op_nosound_nomusic = al_load_bitmap(CAMINHO_IMAGENS "nosound_nomusic.png");
}

void configurar_sprites_personagem() {
	char prefixo[20];
	if (personagem_selecionado == PERSONAGEM_SKATISTA)
		sprintf(prefixo, "dudu_dino");
	else
		sprintf(prefixo, "nina_dina");

	// SOLUÇÃO: Aumente para 256 ou 512
	char buffer[256];

	for (int i = 0; i < 4; i++) {
		if (dino.sprites[i] != NULL) {
			al_destroy_bitmap(dino.sprites[i]);
			dino.sprites[i] = NULL;
		}

		// Use snprintf para garantir que nunca ultrapasse o tamanho do buffer
		snprintf(buffer, sizeof(buffer), "%s%s_%d.png", CAMINHO_IMAGENS, prefixo, i + 1);
		dino.sprites[i] = al_load_bitmap(buffer);

		if (!dino.sprites[i]) {
			printf("ERRO: Nao foi possivel carregar [%s]\n", buffer);
		}
	}
}

void resetar_jogo() {
	configurar_sprites_personagem();

	dino.x = 100.0f;
	dino.y = (float)ALTURA_CHAO;
	dino.vel_y = 0.0f;
	dino.esta_pulando = false;

	for (int i = 0; i < MAX_OBSTACULOS; i++) obstaculos[i].ativo = false;

	itens[0].ativo = false; itens[0].coletado = false; itens[0].tipo = 0;
	itens[1].ativo = false; itens[1].coletado = false; itens[1].tipo = 1;

	itens_coletados = 0;
	frames_decorridos = 0;
	contador_spawn = 0;
	tempo_para_proximo = 30;
	posicao_x_cenario = 0;

	voltas_do_cenario = 0;
	cutscene_iniciada = false;
	cutscene_andando = false;
	encontro_aconteceu = false;
	som_misterio_iniciado = false;

	jogo_com_movimento = false;
}

void atualizar_jogo_geral() {
	if (!jogo_com_movimento) {
		frame_atual_dino = IMG_DINO_CHAO;
		return;
	}

	posicao_x_cenario -= VELOCIDADE_CENARIO;
	float largura_total_loop = 3.0f * largura_fundo;

	if (posicao_x_cenario <= -largura_total_loop) {
		posicao_x_cenario += largura_total_loop;
		voltas_do_cenario++;
	}

	if (voltas_do_cenario >= 10) {
		float pos_abs = -posicao_x_cenario;
		if (pos_abs >= (2 * largura_fundo) - VELOCIDADE_CENARIO) {
			posicao_x_cenario = -(2 * largura_fundo);
			estado_atual = ESTADO_CUTSCENE;
			cutscene_iniciada = true;
			cutscene_andando = false;

			dino.x = -DINO_TELA_W;
			dino.y = ALTURA_CHAO;
			mamae.x = LARGURA_TELA;
			mamae.y = ALTURA_CHAO_MAMAE;
			mamae.frame_atual = 0;

			gerenciar_musica_jogo(ESTADO_CUTSCENE);

			return;
		}
	}

	if (dino.esta_pulando) {
		dino.vel_y += GRAVIDADE;
		dino.y += dino.vel_y;

		if (dino.y >= ALTURA_CHAO) {
			dino.y = (float)ALTURA_CHAO;
			dino.vel_y = 0.0f;
			dino.esta_pulando = false;
		}

		if (dino.vel_y < -3.0f) frame_atual_dino = IMG_DINO_SUBINDO;
		else if (dino.vel_y > 3.0f) frame_atual_dino = IMG_DINO_CAINDO;
		else frame_atual_dino = IMG_DINO_TOPO;
	}
	else {
		frame_atual_dino = IMG_DINO_CHAO;
	}

	if (voltas_do_cenario < 9) {
		contador_spawn++;
		if (contador_spawn >= tempo_para_proximo) {
			for (int i = 0; i < MAX_OBSTACULOS; i++) {
				if (!obstaculos[i].ativo) {
					obstaculos[i].ativo = true;
					obstaculos[i].x = LARGURA_TELA;
					obstaculos[i].y = LINHA_DO_CHAO - OBS_H;
					obstaculos[i].tipo_imagem = rand() % QTD_TIPOS_OBS;
					contador_spawn = 0;
					tempo_para_proximo = 40 + (rand() % 40);
					break;
				}
			}
		}
	}

	for (int i = 0; i < MAX_OBSTACULOS; i++) {
		if (obstaculos[i].ativo) {
			obstaculos[i].x -= VELOCIDADE_CENARIO;
			if (obstaculos[i].x < -150) obstaculos[i].ativo = false;
			if (verificar_colisao_obs(&dino, &obstaculos[i])) {
				estado_atual = ESTADO_GAMEOVER;

				gerenciar_musica_jogo(ESTADO_GAMEOVER);

				if (som_ativado) {
					if (som_colisao) {
						al_play_sample(som_colisao, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &id_som_colisao);
					}
					else {
						fprintf(stderr, "Aviso: som_colisao nao carregado; pulando play.\n");
					}

					if (musica_gameover) {
						al_play_sample(musica_gameover, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
					}
					else {
						fprintf(stderr, "Aviso: musica_gameover nao carregada; pulando play.\n");
					}
				}
			}
		}
	}

	frames_decorridos++;

	if (voltas_do_cenario < 10) {
		if (frames_decorridos == 10 * 60 && !itens[0].coletado) {
			itens[0].ativo = true; itens[0].x = LARGURA_TELA; itens[0].y = (LINHA_DO_CHAO - ITEM_H) - 120;
		}
		if (frames_decorridos == 30 * 60 && !itens[1].coletado) {
			itens[1].ativo = true; itens[1].x = LARGURA_TELA; itens[1].y = (LINHA_DO_CHAO - ITEM_H) - 120;
		}
	}

	for (int i = 0; i < 2; i++) {
		if (itens[i].ativo) {
			itens[i].x -= VELOCIDADE_CENARIO;
			if (verificar_colisao_item(&dino, &itens[i])) {

				tocar_som_evento(som_item_coletado);

				itens[i].ativo = false;
				itens[i].coletado = true;
				itens_coletados++;
			}
			if (itens[i].x < -100) itens[i].ativo = false;
		}
	}
}

void atualizar_cutscene() {
	const float ESPACO_ENTRE_PERSONAGENS = 30.0f;
	const float PONTO_MEIO = LARGURA_TELA / 2.0f;
	const float PONTO_ENCONTRO_DINO = PONTO_MEIO - ESPACO_ENTRE_PERSONAGENS / 2.0f - DINO_TELA_W;
	const float PONTO_ENCONTRO_MAMAE = PONTO_MEIO + ESPACO_ENTRE_PERSONAGENS / 2.0f;

	if (!cutscene_andando) {
		frame_atual_dino = IMG_DINO_CHAO;
		mamae.frame_atual = 0;
		return;
	}

	if (!encontro_aconteceu) {
		bool dino_parou = false;
		bool mamae_parou = false;

		if (dino.x < PONTO_ENCONTRO_DINO) {
			dino.x += 3.0f;
			frame_atual_dino = IMG_DINO_CHAO;
		}
		else {
			dino.x = PONTO_ENCONTRO_DINO;
			dino_parou = true;
		}

		if (mamae.x > PONTO_ENCONTRO_MAMAE) {
			mamae.x -= 3.0f;

			mamae.contador_animacao++;
			if (mamae.contador_animacao > 10) {
				mamae.frame_atual = (mamae.frame_atual + 1) % 3;
				mamae.contador_animacao = 0;
			}
		}
		else {
			mamae.x = PONTO_ENCONTRO_MAMAE;
			mamae.frame_atual = 0;
			mamae_parou = true;
		}

		if (dino_parou && mamae_parou) {
			encontro_aconteceu = true;

			tocar_som_evento(som_encontro);
		}

	}
}

bool verificar_colisao_obs(Dino* d, Obstaculo* obs) {
	float margem_dino_lados = 35.0f;
	float margem_dino_topo = 40.0f;
	float margem_dino_base = 50.0f;
	float margem_obs = 10.0f;
	if ((d->x + margem_dino_lados < obs->x + OBS_W - margem_obs) &&
		(d->x + DINO_TELA_W - margem_dino_lados > obs->x + margem_obs) &&
		(d->y + margem_dino_topo < obs->y + OBS_H) &&
		(d->y + DINO_TELA_H - margem_dino_base > obs->y + margem_obs)) return true;
	return false;
}

bool verificar_colisao_item(Dino* d, Item* it) {
	if ((d->x < it->x + ITEM_W) && (d->x + DINO_TELA_W > it->x) &&
		(d->y < it->y + ITEM_H) && (d->y + DINO_TELA_H > it->y)) return true;
	return false;
}

// ============================= //
// --- 7. FUNÇÕES DE DESENHO --- //
// ============================= //

void desenhar_jogo() {
	if (fundo_jogo[0]) {
		for (int i = 0; i < 6; i++) {
			al_draw_bitmap(fundo_jogo[i % 3], posicao_x_cenario + i * largura_fundo, 0, 0);
		}
	}
	else {
		al_clear_to_color(al_map_rgb(100, 200, 255));
	}

	for (int i = 0; i < MAX_OBSTACULOS; i++) {
		if (obstaculos[i].ativo) {
			ALLEGRO_BITMAP* img = img_obstaculos[obstaculos[i].tipo_imagem];
			if (img) al_draw_scaled_bitmap(img, 0, 0, al_get_bitmap_width(img), al_get_bitmap_height(img), obstaculos[i].x, obstaculos[i].y, OBS_W, OBS_H, 0);
		}
	}

	float flutuacao = sin(al_get_time() * 5.0) * 5.0f;
	for (int i = 0; i < 2; i++) {
		if (itens[i].ativo) {
			float cx = itens[i].x + ITEM_W / 2.0f; float cy = (itens[i].y + ITEM_H / 2.0f) + flutuacao;
			al_draw_filled_circle(cx, cy, (ITEM_W / 2.0f) + 8, al_map_rgb(255, 215, 0));
			ALLEGRO_BITMAP* img = (itens[i].tipo == 0) ? img_batom : img_esmalte;
			if (img) al_draw_scaled_bitmap(img, 0, 0, al_get_bitmap_width(img), al_get_bitmap_height(img), itens[i].x, itens[i].y + flutuacao, ITEM_W, ITEM_H, 0);
		}
	}

	ALLEGRO_BITMAP* sprite_atual = dino.sprites[frame_atual_dino];
	if (sprite_atual) {
		al_draw_scaled_bitmap(sprite_atual, 0, 0, al_get_bitmap_width(sprite_atual), al_get_bitmap_height(sprite_atual),
			dino.x, dino.y, DINO_TELA_W, DINO_TELA_H, 0);
	}
	else {
		al_draw_filled_rectangle(dino.x, dino.y, dino.x + DINO_TELA_W, dino.y + DINO_TELA_H, al_map_rgb(255, 0, 0));
	}

	al_draw_textf(fonte_debug, al_map_rgb(255, 255, 255), 20, 20, 0, "ITENS: %d/2", itens_coletados);

	if (!jogo_com_movimento) {
		al_draw_filled_rectangle(150, 150, 650, 300, al_map_rgba(0, 0, 0, 180));
		al_draw_rectangle(150, 150, 650, 300, al_map_rgb(255, 255, 255), 3);

		al_draw_text(fonte_debug, al_map_rgb(255, 255, 0), LARGURA_TELA / 2, 180, ALLEGRO_ALIGN_CENTER, "ME AJUDE COM O IMPULSO!");
		al_draw_text(fonte_debug, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, 220, ALLEGRO_ALIGN_CENTER, "Pressione a SETA DIREITA (->)");
		al_draw_text(fonte_debug, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, 240, ALLEGRO_ALIGN_CENTER, "para eu conseguir andar");
	}
}

void desenhar_cutscene() {
	if (fundo_jogo[2]) al_draw_bitmap(fundo_jogo[2], 0, 0, 0);
	ALLEGRO_BITMAP* sprite_dino = dino.sprites[IMG_DINO_CHAO];
	if (sprite_dino) {
		al_draw_scaled_bitmap(sprite_dino, 0, 0, al_get_bitmap_width(sprite_dino), al_get_bitmap_height(sprite_dino),
			dino.x, dino.y, DINO_TELA_W, DINO_TELA_H, 0);
	}

	ALLEGRO_BITMAP* sprite_mamae = mamae.sprites[mamae.frame_atual];
	if (sprite_mamae) {
		al_draw_scaled_bitmap(sprite_mamae, 0, 0, al_get_bitmap_width(sprite_mamae), al_get_bitmap_height(sprite_mamae),
			mamae.x, ALTURA_CHAO_MAMAE, MAMAE_W, MAMAE_H, 0);
	}

	if (encontro_aconteceu) {
		al_draw_text(fonte_debug, al_map_rgb(0, 0, 0), LARGURA_TELA / 2 + 1, 150, ALLEGRO_ALIGN_CENTER, "MAMAE, TE ENCONTREI!");
		al_draw_text(fonte_debug, al_map_rgb(255, 255, 255), LARGURA_TELA / 2 + 3, 150, ALLEGRO_ALIGN_CENTER, "MAMAE, TE ENCONTREI! ");
		al_draw_text(fonte_debug, al_map_rgb(0, 0, 0), LARGURA_TELA / 2 + 1, 180 + 1, ALLEGRO_ALIGN_CENTER, "CLIQUE EM ENTER PARA CONTINUAR! ");
		al_draw_text(fonte_debug, al_map_rgb(255, 255, 0), LARGURA_TELA / 2, 180, ALLEGRO_ALIGN_CENTER, "CLIQUE EM ENTER PARA CONTINUAR! ");

		if (img_coracao) {
			float LARGURA_CORACAO = 80.0f;
			float ALTURA_CORACAO = 80.0f;
			float w_orig = al_get_bitmap_width(img_coracao);
			float h_orig = al_get_bitmap_height(img_coracao);
			float pos_x = (LARGURA_TELA / 2) - (LARGURA_CORACAO / 2);
			float pos_y = dino.y - ALTURA_CORACAO - 20;

			al_draw_scaled_bitmap(img_coracao,
				0, 0, w_orig, h_orig,
				pos_x, pos_y,
				LARGURA_CORACAO, ALTURA_CORACAO,
				0);
		}
		else {
			float cx = (LARGURA_TELA / 2.0f);
			float cy = dino.y - 10;
			ALLEGRO_COLOR cor_coracao = al_map_rgb(255, 0, 0);
			ALLEGRO_COLOR cor_borda = al_map_rgb(255, 0, 0);
			float pulso = sin(al_get_time() * 10.0) * 2.0f;
			float r = 25.0f;

			float fator_x_base = 2.0f;
			float fator_y_ponta = 2.0f;

			al_draw_filled_circle(cx - r + pulso, cy + pulso, r, cor_coracao);
			al_draw_circle(cx - r + pulso, cy + pulso, r, cor_borda, 1.0);
			al_draw_filled_circle(cx + r + pulso, cy + pulso, r, cor_coracao);
			al_draw_circle(cx + r + pulso, cy + pulso, r, cor_borda, 1.0);

			al_draw_filled_triangle(cx - fator_x_base * r + pulso, cy + 0.5 * r + pulso, cx + fator_x_base * r + pulso, cy + 0.5 * r + pulso, cx + pulso, cy + fator_y_ponta * r + pulso, cor_coracao);
			al_draw_triangle(cx - fator_x_base * r + pulso, cy + 0.5 * r + pulso, cx + fator_x_base * r + pulso, cy + 0.5 * r + pulso, cx + pulso, cy + fator_y_ponta * r + pulso, cor_borda, 1.0);

			al_draw_text(fonte_debug, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, 50, ALLEGRO_ALIGN_CENTER, "IMAGEM CORACAO FALTANDO (DEBUG)");
		}
	}
	else if (!cutscene_andando) {

		if (!som_misterio_iniciado) {
			tocar_som_evento(som_misterio);
			som_misterio_iniciado = true;
		}

		al_draw_text(fonte_debug, al_map_rgb(0, 0, 0), LARGURA_TELA / 2 + 1, 202, ALLEGRO_ALIGN_CENTER, "A MAMAE ESTA POR PERTO!");
		al_draw_text(fonte_debug, al_map_rgb(255, 215, 0), LARGURA_TELA / 2 + 3, 200, ALLEGRO_ALIGN_CENTER, "A MAMAE ESTA POR PERTO!");
		al_draw_text(fonte_debug, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, 230, ALLEGRO_ALIGN_CENTER, "[PRESSIONE ENTER]");
	}
}

void desenhar_gameover() {
	ALLEGRO_BITMAP* img_go = (personagem_selecionado == PERSONAGEM_PATINADORA) ? img_gameover_nina : img_gameover_dudu;

	if (img_go) {
		al_draw_scaled_bitmap(img_go, 0, 0, al_get_bitmap_width(img_go), al_get_bitmap_height(img_go), 0, 0, LARGURA_TELA, ALTURA_TELA, 0);
	}
	else {
		al_clear_to_color(al_map_rgb(100, 0, 0));
		al_draw_text(fonte_debug, al_map_rgb(255, 255, 255), 400, 300, ALLEGRO_ALIGN_CENTER, "GAME OVER - ENTER PARA MENU");
	}
}

void desenhar_vitoria() {
	ALLEGRO_COLOR cor_hover = al_map_rgb(180, 180, 180);
	ALLEGRO_COLOR cor_click_sel = al_map_rgb(255, 255, 0);
	ALLEGRO_COLOR cor_borda_normal = al_map_rgba(0, 0, 0, 0);

	if (img_vitoria) {
		al_draw_scaled_bitmap(img_vitoria, 0, 0, al_get_bitmap_width(img_vitoria), al_get_bitmap_height(img_vitoria), 0, 0, LARGURA_TELA, ALTURA_TELA, 0);
	}
	else {
		al_clear_to_color(al_map_rgb(50, 150, 50));
		al_draw_text(fonte_debug, al_map_rgb(0, 0, 0), LARGURA_TELA / 2, 200, ALLEGRO_ALIGN_CENTER, "VOCÊ VENCEU! (Imagem não carregada)");
		al_draw_text(fonte_debug, al_map_rgb(0, 0, 0), LARGURA_TELA / 2, 250, ALLEGRO_ALIGN_CENTER, "Aperte ENTER ou clique no botão para voltar.");
	}

	Botao* botoes_vitoria[] = { &botao_vitoria_playagain, &botao_vitoria_returnmenu, &botao_vitoria_nextlevel };

	for (int i = 0; i < 3; i++) {
		Botao* b = botoes_vitoria[i];
		ALLEGRO_COLOR cor_borda = cor_borda_normal;

		if (mouse_sobre_botao(b, mouse_x, mouse_y)) {
			cor_borda = mouse_botao_pressionado ? cor_click_sel : cor_hover;
		}

		if (cor_borda.a > 0 || cor_borda.r > 0 || cor_borda.g > 0 || cor_borda.b > 0) {
			al_draw_rectangle(b->x, b->y, b->x + b->largura, b->y + b->altura, cor_borda, 3);
		}
	}
}

// ================================== //
// --- 8. FUNÇÃO PRINCIPAL (MAIN) --- //
// ================================== //

int main(void) {
	srand((unsigned int)time(NULL));

	if (!al_init() || !al_init_primitives_addon() || !al_install_keyboard() ||
		!al_install_mouse() || !al_init_image_addon() ||
		!al_install_audio() || !al_init_acodec_addon()) {
		fprintf(stderr, "Falha na inicializacao Allegro.\n");
		return -1;
	}
	al_init_font_addon();
	al_init_ttf_addon();
	al_reserve_samples(15);

	janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
	timer = al_create_timer(1.0 / FPS);
	fila_eventos = al_create_event_queue();
	fonte_debug = al_load_ttf_font("arial.ttf", 18, 0);
	if (!fonte_debug) {
		fonte_debug = al_create_builtin_font();
	}
	al_set_window_title(janela, "Slide Dino");

	fundo_menu_bitmap = al_load_bitmap(CAMINHO_IMAGENS "menu_background.png");
	tela_selecao_bitmap = al_load_bitmap(CAMINHO_IMAGENS "menu_selecao_personagem.png");
	historia_dudu_bitmap = al_load_bitmap(CAMINHO_IMAGENS "dudu_dino_story.png");
	historia_nina_bitmap = al_load_bitmap(CAMINHO_IMAGENS "nina_dina_story.png");

	som_mouse_click = al_load_sample(CAMINHO_SONS "barulho_mouse.mp3");
	if (!som_mouse_click) fprintf(stderr, "Aviso: falha ao carregar 'barulho_mouse.mp3'\n");
	som_fala_dino = al_load_sample(CAMINHO_SONS "fala_dino.mp3");
	if (!som_fala_dino) fprintf(stderr, "Aviso: falha ao carregar 'fala_dino.mp3'\n");
	musica_menu_inicial = al_load_audio_stream(CAMINHO_SONS "menu_inicial.mp3", 4, 2048);
	if (!musica_menu_inicial) fprintf(stderr, "Aviso: falha ao carregar 'menu_inicial.mp3'\n");
	musica_selecao = al_load_audio_stream(CAMINHO_SONS "menu_selecao.mp3", 4, 2048);
	if (!musica_selecao) fprintf(stderr, "Aviso: falha ao carregar 'menu_selecao.mp3'\n");
	musica_opcoes = al_load_audio_stream(CAMINHO_SONS "menu_settings.mp3", 4, 2048);
	if (!musica_opcoes) fprintf(stderr, "Aviso: falha ao carregar 'menu_settings.mp3'\n");

	som_pulo = al_load_sample(CAMINHO_SONS "som_pulo.mp3");
	if (!som_pulo) fprintf(stderr, "Aviso: falha ao carregar 'som_pulo.mp3'\n");
	som_colisao = al_load_sample(CAMINHO_SONS "som_colisão.mp3");
	if (!som_colisao) {
		som_colisao = al_load_sample(CAMINHO_SONS "som_colisao.mp3");
		if (som_colisao) fprintf(stderr, "Aviso: carregado 'som_colisao.mp3' (fallback sem acento)\n");
		else fprintf(stderr, "Aviso: falha ao carregar 'som_colisão.mp3' ou 'som_colisao.mp3'\n");
	}
	musica_jogo = al_load_audio_stream(CAMINHO_SONS "loop_jogo.mp3", 4, 2048);
	if (!musica_jogo) fprintf(stderr, "Aviso: falha ao carregar 'loop_jogo.mp3'\n");
	musica_gameover = al_load_sample(CAMINHO_SONS "som_tela_gameover.mp3");
	if (!musica_gameover) fprintf(stderr, "Aviso: falha ao carregar 'som_tela_gameover.mp3'\n");
	musica_vitoria = al_load_sample(CAMINHO_SONS "som_tela_win.mp3");
	if (!musica_vitoria) fprintf(stderr, "Aviso: falha ao carregar 'som_tela_win.mp3'\n");

	carregar_recursos_jogo();

	al_register_event_source(fila_eventos, al_get_display_event_source(janela));
	al_register_event_source(fila_eventos, al_get_timer_event_source(timer));
	al_register_event_source(fila_eventos, al_get_keyboard_event_source());
	al_register_event_source(fila_eventos, al_get_mouse_event_source());

	botao_start = (Botao){ 240, 328, 317, 80 };
	botao_options = (Botao){ 240, 418 , 317, 80 };
	botao_exit = (Botao){ 240, 506, 317, 80 };
	botao_continue_selecao = (Botao){ 240, 398, 317, 80 };
	botao_return_menu = (Botao){ 240, 489, 317, 80 };
	area_dino_skate = (Botao){ 117, 165, 235, 192 };
	area_dino_patins = (Botao){ 445, 165, 235, 192 };
	botao_yes_i_do = (Botao){ 261, 251, 129, 34 };
	botao_no_i_dont = (Botao){ 410, 251, 129, 34 };
	botao_return_historia = (Botao){ 266, 450, 266, 67 };

	const float X_SLIDER = 340, Y_SFX = 170, Y_MUS = 286, W_SL = 250, H_SL = 60;
	area_click_sfx_off = (Botao){ X_SLIDER, Y_SFX, W_SL / 2, H_SL };
	area_click_sfx_on = (Botao){ X_SLIDER + W_SL / 2, Y_SFX, W_SL / 2, H_SL };
	area_click_music_off = (Botao){ X_SLIDER, Y_MUS, W_SL / 2, H_SL };
	area_click_music_on = (Botao){ X_SLIDER + W_SL / 2, Y_MUS, W_SL / 2, H_SL };
	botao_confirmar = (Botao){ 241, 400, 316, 79 };
	botao_retornar_menu = (Botao){ 241, 490, 316, 79 };

	botao_go_tryagain = (Botao){ 302, 268, 176, 46 };
	botao_go_menu = (Botao){ 240, 445, 316, 77 };

	botao_vitoria_playagain = (Botao){ 242, 268, 314, 77 };
	botao_vitoria_returnmenu = (Botao){ 242, 362, 314, 77 };
	botao_vitoria_nextlevel = (Botao){ 229, 454, 339, 83 };

	al_start_timer(timer);
	gerenciar_musica_menu(ESTADO_MENU);

	while (rodando) {

		if (estado_atual == ESTADO_HISTORIA && som_fala_dino && !fala_dino_iniciada) {
			al_play_sample(som_fala_dino, 0.6 * (som_ativado ? 1.0 : 0.0), 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &id_fala_dino);
			fala_dino_iniciada = true;
		}
		else if (estado_atual != ESTADO_HISTORIA && fala_dino_iniciada) {
			al_stop_sample(&id_fala_dino);
			fala_dino_iniciada = false;
		}

		ALLEGRO_EVENT evento;
		al_wait_for_event(fila_eventos, &evento);

		if (evento.type == ALLEGRO_EVENT_TIMER) {
			if (estado_atual == ESTADO_EM_JOGO) {
				atualizar_jogo_geral();
			}
			else if (estado_atual == ESTADO_CUTSCENE) {
				atualizar_cutscene();
			}
			desenhar = true;
		}
		else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			rodando = false;
		}
		else if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
			if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
				if (estado_atual == ESTADO_EM_JOGO || estado_atual == ESTADO_SELECAO || estado_atual == ESTADO_OPCOES || estado_atual == ESTADO_GAMEOVER || estado_atual == ESTADO_CUTSCENE || estado_atual == ESTADO_VITORIA) {
					gerenciar_musica_jogo(ESTADO_MENU);
					gerenciar_musica_vitoria(ESTADO_MENU);
					gerenciar_musica_selecao(ESTADO_MENU);
					gerenciar_musica_opcoes(ESTADO_MENU);
					estado_atual = ESTADO_MENU;
					personagem_selecionado = PERSONAGEM_NENHUM;
					gerenciar_musica_menu(ESTADO_MENU);
				}
				else if (estado_atual == ESTADO_MENU) {
					rodando = false;
				}
			}
			if (evento.keyboard.keycode == ALLEGRO_KEY_ENTER) {
				if (estado_atual == ESTADO_GAMEOVER) {
					estado_atual = ESTADO_MENU;
					personagem_selecionado = PERSONAGEM_NENHUM;
					gerenciar_musica_menu(ESTADO_MENU);
				}
				else if (estado_atual == ESTADO_CUTSCENE) {
					if (!cutscene_andando) cutscene_andando = true;
					else if (encontro_aconteceu) {
						estado_atual = ESTADO_VITORIA;
						gerenciar_musica_vitoria(ESTADO_VITORIA);
					}
				}
				else if (estado_atual == ESTADO_VITORIA) {
					estado_atual = ESTADO_MENU;
					personagem_selecionado = PERSONAGEM_NENHUM;
					gerenciar_musica_vitoria(ESTADO_MENU);
					gerenciar_musica_menu(ESTADO_MENU);
				}
			}

			if (estado_atual == ESTADO_EM_JOGO) {
				if (!jogo_com_movimento) {
					if (evento.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
						jogo_com_movimento = true;
					}
				}
				else {
					if (evento.keyboard.keycode == ALLEGRO_KEY_UP || evento.keyboard.keycode == ALLEGRO_KEY_SPACE || evento.keyboard.keycode == ALLEGRO_KEY_W) {
						if (!dino.esta_pulando) {
							tocar_som_evento(som_pulo);
							dino.esta_pulando = true;
							dino.vel_y = VELOCIDADE_PULO;
						}
					}
				}
			}
		}
		else if (evento.type == ALLEGRO_EVENT_MOUSE_AXES) {
			mouse_x = evento.mouse.x;
			mouse_y = evento.mouse.y;
		}
		else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			if (evento.mouse.button & 1) {
				mouse_botao_pressionado = true;
			}
		}
		else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
			if (evento.mouse.button & 1 && mouse_botao_pressionado) {

				if (estado_atual == ESTADO_MENU) {
					if (mouse_sobre_botao(&botao_start, mouse_x, mouse_y)) {
						tocar_clique();
						estado_atual = ESTADO_SELECAO;
						gerenciar_musica_menu(ESTADO_SELECAO); gerenciar_musica_selecao(ESTADO_SELECAO);
					}
					else if (mouse_sobre_botao(&botao_options, mouse_x, mouse_y)) {
						tocar_clique();
						estado_atual = ESTADO_OPCOES;
						gerenciar_musica_menu(ESTADO_OPCOES); gerenciar_musica_opcoes(ESTADO_OPCOES);
					}
					else if (mouse_sobre_botao(&botao_exit, mouse_x, mouse_y)) {
						tocar_clique();
						rodando = false;
					}
				}
				else if (estado_atual == ESTADO_SELECAO) {
					if (mouse_sobre_botao(&area_dino_skate, mouse_x, mouse_y)) {
						tocar_clique();
						personagem_selecionado = PERSONAGEM_SKATISTA;
					}
					else if (mouse_sobre_botao(&area_dino_patins, mouse_x, mouse_y)) {
						tocar_clique();
						personagem_selecionado = PERSONAGEM_PATINADORA;
					}
					else if (mouse_sobre_botao(&botao_continue_selecao, mouse_x, mouse_y) && personagem_selecionado != PERSONAGEM_NENHUM) {
						tocar_clique();
						estado_atual = ESTADO_HISTORIA; gerenciar_musica_selecao(ESTADO_HISTORIA);
					}
					else if (mouse_sobre_botao(&botao_return_menu, mouse_x, mouse_y)) {
						tocar_clique();
						estado_atual = ESTADO_MENU;
						personagem_selecionado = PERSONAGEM_NENHUM;
						gerenciar_musica_selecao(ESTADO_MENU); gerenciar_musica_menu(ESTADO_MENU);
					}
				}
				else if (estado_atual == ESTADO_HISTORIA) {
					if (mouse_sobre_botao(&botao_yes_i_do, mouse_x, mouse_y)) {
						tocar_clique();
						estado_atual = ESTADO_EM_JOGO;
						gerenciar_musica_selecao(ESTADO_MENU);
						gerenciar_musica_jogo(ESTADO_EM_JOGO);
						resetar_jogo();
					}
					else if (mouse_sobre_botao(&botao_no_i_dont, mouse_x, mouse_y)) {
						tocar_clique();
						estado_atual = ESTADO_MENU;
						personagem_selecionado = PERSONAGEM_NENHUM;
						gerenciar_musica_menu(ESTADO_MENU);
					}
					else if (mouse_sobre_botao(&botao_return_historia, mouse_x, mouse_y)) {
						tocar_clique();
						estado_atual = ESTADO_SELECAO; gerenciar_musica_selecao(ESTADO_SELECAO);
					}
				}
				else if (estado_atual == ESTADO_OPCOES) {
					bool clicked_on_button = false;
					if (som_ativado && mouse_sobre_botao(&area_click_sfx_off, mouse_x, mouse_y)) { som_ativado = false; clicked_on_button = true; }
					else if (!som_ativado && mouse_sobre_botao(&area_click_sfx_on, mouse_x, mouse_y)) { som_ativado = true; clicked_on_button = true; }

					if (musica_ativada && mouse_sobre_botao(&area_click_music_off, mouse_x, mouse_y)) {
						musica_ativada = false;
						if (musica_opcoes_tocando) { al_set_audio_stream_playing(musica_opcoes, false); musica_opcoes_tocando = false; }
						clicked_on_button = true;
					}
					else if (!musica_ativada && mouse_sobre_botao(&area_click_music_on, mouse_x, mouse_y)) {
						musica_ativada = true; gerenciar_musica_opcoes(ESTADO_OPCOES);
						clicked_on_button = true;
					}

					if (mouse_sobre_botao(&botao_confirmar, mouse_x, mouse_y)) {
						gerenciar_musica_opcoes(ESTADO_MENU); estado_atual = ESTADO_MENU; gerenciar_musica_menu(ESTADO_MENU);
						clicked_on_button = true;
					}
					else if (mouse_sobre_botao(&botao_retornar_menu, mouse_x, mouse_y)) {
						gerenciar_musica_opcoes(ESTADO_MENU); estado_atual = ESTADO_MENU; gerenciar_musica_menu(ESTADO_MENU);
						clicked_on_button = true;
					}

					if (clicked_on_button) tocar_clique();
				}
				else if (estado_atual == ESTADO_GAMEOVER) {
					if (mouse_sobre_botao(&botao_go_tryagain, mouse_x, mouse_y)) {
						tocar_clique();
						gerenciar_musica_jogo(ESTADO_EM_JOGO);
						resetar_jogo();
						estado_atual = ESTADO_EM_JOGO;
					}
					else if (mouse_sobre_botao(&botao_go_menu, mouse_x, mouse_y)) {
						tocar_clique();
						estado_atual = ESTADO_MENU;
						personagem_selecionado = PERSONAGEM_NENHUM;
						gerenciar_musica_menu(ESTADO_MENU);
					}
				}
				else if (estado_atual == ESTADO_VITORIA) {
					if (mouse_sobre_botao(&botao_vitoria_playagain, mouse_x, mouse_y)) {
						tocar_clique();
						gerenciar_musica_vitoria(ESTADO_EM_JOGO);
						gerenciar_musica_jogo(ESTADO_EM_JOGO);
						resetar_jogo();
						estado_atual = ESTADO_EM_JOGO;
					}
					else if (mouse_sobre_botao(&botao_vitoria_returnmenu, mouse_x, mouse_y)) {
						tocar_clique();
						estado_atual = ESTADO_MENU;
						personagem_selecionado = PERSONAGEM_NENHUM;
						gerenciar_musica_vitoria(ESTADO_MENU);
						gerenciar_musica_menu(ESTADO_MENU);
					}
					else if (mouse_sobre_botao(&botao_vitoria_nextlevel, mouse_x, mouse_y)) {
						tocar_clique();
					}
				}
			}
			mouse_botao_pressionado = false;
		}

		if (desenhar && al_is_event_queue_empty(fila_eventos)) {
			desenhar = false;
			al_clear_to_color(al_map_rgb(0, 0, 0));

			ALLEGRO_COLOR cor_hover = al_map_rgb(180, 180, 180);
			ALLEGRO_COLOR cor_click_sel = al_map_rgb(255, 255, 0);
			float espessura_padrao = 4;

			if (estado_atual == ESTADO_MENU) {
				if (fundo_menu_bitmap) al_draw_scaled_bitmap(fundo_menu_bitmap, 0, 0, al_get_bitmap_width(fundo_menu_bitmap), al_get_bitmap_height(fundo_menu_bitmap), 0, 0, LARGURA_TELA, ALTURA_TELA, 0);

				Botao* botoes[] = { &botao_start, &botao_options, &botao_exit };
				for (int i = 0; i < 3; i++) {
					if (mouse_sobre_botao(botoes[i], mouse_x, mouse_y)) {
						ALLEGRO_COLOR cor = mouse_botao_pressionado ? cor_click_sel : cor_hover;
						al_draw_rectangle(botoes[i]->x, botoes[i]->y, botoes[i]->x + botoes[i]->largura, botoes[i]->y + botoes[i]->altura, cor, espessura_padrao);
					}
				}
			}
			else if (estado_atual == ESTADO_SELECAO) {
				if (tela_selecao_bitmap) al_draw_scaled_bitmap(tela_selecao_bitmap, 0, 0, al_get_bitmap_width(tela_selecao_bitmap), al_get_bitmap_height(tela_selecao_bitmap), 0, 0, LARGURA_TELA, ALTURA_TELA, 0);

				Botao* areas_personagem[] = { &area_dino_skate, &area_dino_patins };
				Personagem personagens[] = { PERSONAGEM_SKATISTA, PERSONAGEM_PATINADORA };

				for (int i = 0; i < 2; i++) {
					ALLEGRO_COLOR cor;
					if (personagem_selecionado == personagens[i]) {
						cor = cor_click_sel;
					}
					else if (mouse_sobre_botao(areas_personagem[i], mouse_x, mouse_y)) {
						cor = mouse_botao_pressionado ? cor_click_sel : cor_hover;
					}
					else {
						continue;
					}
					al_draw_rectangle(areas_personagem[i]->x, areas_personagem[i]->y,
						areas_personagem[i]->x + areas_personagem[i]->largura,
						areas_personagem[i]->y + areas_personagem[i]->altura, cor, 5);
				}

				if (mouse_sobre_botao(&botao_continue_selecao, mouse_x, mouse_y)) {
					ALLEGRO_COLOR cor = mouse_botao_pressionado ? cor_click_sel : cor_hover;
					al_draw_rectangle(botao_continue_selecao.x, botao_continue_selecao.y,
						botao_continue_selecao.x + botao_continue_selecao.largura,
						botao_continue_selecao.y + botao_continue_selecao.altura, cor, espessura_padrao);
				}

				if (mouse_sobre_botao(&botao_return_menu, mouse_x, mouse_y)) {
					ALLEGRO_COLOR cor = mouse_botao_pressionado ? cor_click_sel : cor_hover;
					al_draw_rectangle(botao_return_menu.x, botao_return_menu.y,
						botao_return_menu.x + botao_return_menu.largura,
						botao_return_menu.y + botao_return_menu.altura, cor, espessura_padrao);
				}

				if (mouse_sobre_botao(&botao_continue_selecao, mouse_x, mouse_y) && personagem_selecionado == PERSONAGEM_NENHUM) {
					al_draw_text(fonte_debug, al_map_rgb(255, 0, 0), LARGURA_TELA / 2, 385, ALLEGRO_ALIGN_CENTER, "Selecione um personagem!");
				}
			}
			else if (estado_atual == ESTADO_HISTORIA) {
				ALLEGRO_BITMAP* hist = (personagem_selecionado == PERSONAGEM_SKATISTA) ? historia_dudu_bitmap : historia_nina_bitmap;
				if (hist) al_draw_scaled_bitmap(hist, 0, 0, al_get_bitmap_width(hist), al_get_bitmap_height(hist), 0, 0, LARGURA_TELA, ALTURA_TELA, 0);

				Botao* botoes_historia[] = { &botao_yes_i_do, &botao_no_i_dont, &botao_return_historia };
				for (int i = 0; i < 3; i++) {
					if (mouse_sobre_botao(botoes_historia[i], mouse_x, mouse_y)) {
						ALLEGRO_COLOR cor = mouse_botao_pressionado ? cor_click_sel : cor_hover;
						al_draw_rectangle(botoes_historia[i]->x, botoes_historia[i]->y,
							botoes_historia[i]->x + botoes_historia[i]->largura,
							botoes_historia[i]->y + botoes_historia[i]->altura, cor, espessura_padrao);
					}
				}
			}
			else if (estado_atual == ESTADO_OPCOES) {
				ALLEGRO_BITMAP* tela_op = NULL;

				if (som_ativado && musica_ativada) tela_op = op_sound_music;
				else if (som_ativado && !musica_ativada) tela_op = op_sound_nomusic;
				else if (!som_ativado && musica_ativada) tela_op = op_nosound_music;
				else tela_op = op_nosound_nomusic;

				if (tela_op) {
					al_draw_scaled_bitmap(tela_op, 0, 0, al_get_bitmap_width(tela_op), al_get_bitmap_height(tela_op), 0, 0, LARGURA_TELA, ALTURA_TELA, 0);
				}

				Botao* botoes_opcoes_principais[] = { &botao_confirmar, &botao_retornar_menu };
				for (int i = 0; i < 2; i++) {
					if (mouse_sobre_botao(botoes_opcoes_principais[i], mouse_x, mouse_y)) {
						ALLEGRO_COLOR cor = mouse_botao_pressionado ? cor_click_sel : cor_hover;
						al_draw_rectangle(botoes_opcoes_principais[i]->x, botoes_opcoes_principais[i]->y,
							botoes_opcoes_principais[i]->x + botoes_opcoes_principais[i]->largura,
							botoes_opcoes_principais[i]->y + botoes_opcoes_principais[i]->altura, cor, espessura_padrao);
					}
				}
			}
			else if (estado_atual == ESTADO_EM_JOGO) {
				desenhar_jogo();
			}
			else if (estado_atual == ESTADO_CUTSCENE) {
				desenhar_cutscene();
			}
			else if (estado_atual == ESTADO_GAMEOVER) {
				desenhar_gameover();

				Botao* botoes_go[] = { &botao_go_tryagain, &botao_go_menu };
				for (int i = 0; i < 2; i++) {
					if (mouse_sobre_botao(botoes_go[i], mouse_x, mouse_y)) {
						ALLEGRO_COLOR cor = mouse_botao_pressionado ? cor_click_sel : cor_hover;
						al_draw_rectangle(botoes_go[i]->x, botoes_go[i]->y,
							botoes_go[i]->x + botoes_go[i]->largura,
							botoes_go[i]->y + botoes_go[i]->altura, cor, espessura_padrao);
					}
				}
			}
			else if (estado_atual == ESTADO_VITORIA) {
				desenhar_vitoria();
			}

			al_flip_display();
		}
	}

	al_destroy_bitmap(fundo_menu_bitmap);
	al_destroy_bitmap(tela_selecao_bitmap);
	al_destroy_bitmap(historia_dudu_bitmap);
	al_destroy_bitmap(historia_nina_bitmap);
	if (img_vitoria) al_destroy_bitmap(img_vitoria);
	if (img_coracao) al_destroy_bitmap(img_coracao);
	if (fonte_debug && fonte_debug != al_create_builtin_font()) al_destroy_font(fonte_debug);

	for (int i = 0; i < 3; i++) if (fundo_jogo[i]) al_destroy_bitmap(fundo_jogo[i]);
	for (int i = 0; i < QTD_TIPOS_OBS; i++) if (img_obstaculos[i]) al_destroy_bitmap(img_obstaculos[i]);
	for (int i = 0; i < 4; i++) if (dino.sprites[i]) al_destroy_bitmap(dino.sprites[i]);
	for (int i = 0; i < 3; i++) if (mamae.sprites[i]) al_destroy_bitmap(mamae.sprites[i]);
	if (img_batom) al_destroy_bitmap(img_batom);
	if (img_esmalte) al_destroy_bitmap(img_esmalte);
	if (img_gameover_nina) al_destroy_bitmap(img_gameover_nina);
	if (img_gameover_dudu) al_destroy_bitmap(img_gameover_dudu);

	if (op_sound_music) al_destroy_bitmap(op_sound_music);
	if (op_sound_nomusic) al_destroy_bitmap(op_sound_nomusic);
	if (op_nosound_music) al_destroy_bitmap(op_nosound_music);
	if (op_nosound_nomusic) al_destroy_bitmap(op_nosound_nomusic);

	if (musica_menu_inicial) al_destroy_audio_stream(musica_menu_inicial);
	if (musica_selecao) al_destroy_audio_stream(musica_selecao);
	if (musica_opcoes) al_destroy_audio_stream(musica_opcoes);
	if (musica_jogo) al_destroy_audio_stream(musica_jogo);
	if (musica_gameover) al_destroy_sample(musica_gameover);
	if (musica_vitoria) al_destroy_sample(musica_vitoria);
	if (som_pulo) al_destroy_sample(som_pulo);
	if (som_colisao) al_destroy_sample(som_colisao);
	if (som_fala_dino) al_destroy_sample(som_fala_dino);
	if (som_mouse_click) al_destroy_sample(som_mouse_click);

	if (som_item_coletado) al_destroy_sample(som_item_coletado);
	if (som_encontro) al_destroy_sample(som_encontro);

	al_destroy_display(janela);
	al_destroy_timer(timer);
	al_destroy_event_queue(fila_eventos);

	return 0;
}
