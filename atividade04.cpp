/*
Esse codigo serve para vc plotar 3 curvas:

1) uma parabola (que ja plota automatico), apenas para voce estudar o codigo e ver como plota-se uma curva no OpenGL

2) Uma Curva de Hermite, cujas tangentes de entrada e saida sao fixos com #define. Os pontos devem ser
fornecidos pelo usuário na interface. E a sua tarefa é plotar a curva com esses pontos


2) Uma Curva de Bezier de grau 2, cujos pontos de controle devem ser
fornecidos pelo usuário na interface. E a sua tarefa é plotar a curva com esses pontos

Esse codigo foi explicado na aula do dia 04/09/2020.
*/

#include <iostream>
#include <vector>
#include <math.h>


// Freeglut library
#include <GL/freeglut.h>
#include "GL\glut.h"

// Domínio e Contra-Dominio Cartesiano 
// para o Exercicio de Curvas e Superficies
#define MAIORX  400.0f
#define MENORX  0.0f
#define MAIORY  400.0f
#define MENORY  0.0f

// largua e altura das dimensões da 
// Janela do Windows
#define LARGURA_WINDOW 400
#define ALTURA_WINDOW  400

// posição inicial 
// (canto esquerdo superior) da 
// janela Windows 
#define POSWX 250
#define POSWY 150

// tangentes iniciais e finais dos 
// dois pontos da curva de Hermite

// 2021-04-08 - curvas em aula
// "sorriso"
// T0 = (0, -1000)
// T1 = (0,  1000)

// "S"
// T0 = (100,  0)
// T1 = ( 50, 50)


#define tang1x 1414.2f
#define tang1y    0.0f 
#define tang2x 1000.0f
#define tang2y 1000.0f 

// Polinomio de Bezier e Hermite
// para Bezier
GLfloat PB[4][2];

// para Hermite
GLfloat PH[2][4];

// variaveis de controle
// GET_POINTS = 0 não está pegando pontos;
// GET_POINTS = 1, está pegando pontos
GLboolean GET_POINTS = 0;

// total de pontos das curvas de 
// Bezier e Hermite
GLint     TOTAL_POINTS = 0;

// pontos inseridos para as curvas
GLfloat bx, by;

// função de inicialização
void Inicializa(void);

// função calback de ajuste da 
// janela de visualização
void AlteraTamanhoJanela(GLsizei w,
	GLsizei h);

// função callback de teclado
void Teclado(unsigned char key, int x, int y);

// função callback principal de desenho
// (chama todas as outras)
void Desenha(void);

//função callback de mouse para 
// o exercício de curvas
void myMouseFuncIterate(int button,
	int state, int x, int y);

// função que multiplica 
// as matrizes
GLfloat multiply(GLfloat T[],
	GLfloat M[][4], GLfloat P[]);

// desenha uma parabola simples
void DesenhaParabola();

// desenha uma curva de hermite de grau 3
void DesenhaHermiteGrau3();

void DesenhaBezierGrau3();

void DesenhaEixos();

enum class Curva {
	BEZIER,
	HERMITE,
	PARABOLA
};

Curva CURVA = Curva::HERMITE;

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	// Define do modo de operação da GLUT
	// GLUT_SINGLE significa que vai usar 
	// 1 buffer só 
	// (se for usar animação ou 3D 
	//  use GLUT_DOUBLE) aqui, 
	// como o código é 2D usamos 1 buffer
	// GLUT_RGB significa que cada pixel 
	// é RGB
	glutInitDisplayMode(GLUT_SINGLE
		| GLUT_RGB);

	// Especifica a posição inicial da 
	// janela GLUT
	glutInitWindowPosition(POSWX, POSWY);

	// Especifica o tamanho inicial 
	// em pixels da janela GLUT
	glutInitWindowSize(LARGURA_WINDOW,
		ALTURA_WINDOW);

	std::string nome = "";
	switch (CURVA)
	{
	case Curva::BEZIER:
		nome = "curva de Bezier"; break;
	case Curva::HERMITE:
		nome = "curva de Hermite"; break;
	case Curva::PARABOLA:
		nome = "parabola"; break;
	default: break;
	}
	// Cria a janela passando 
	// como argumento o título da mesma
	std::string titulo = "Uma bela " + nome;
	glutCreateWindow(titulo.c_str());

	// Registra a função callback de 
	// redesenho da janela de visualização
	glutDisplayFunc(Desenha);

	// Registra a função callback de 
	// redimensionamento da 
	// janela de visualização
	glutReshapeFunc(AlteraTamanhoJanela);

	// Registra a função callback para 
	// tratamento das teclas ASCII
	glutKeyboardFunc(Teclado);

	// Registra a função callback mouse
	glutMouseFunc(myMouseFuncIterate);

	// Chama a função responsável por 
	// fazer as inicializações 
	Inicializa();

	// Inicia o processamento e 
	// aguarda interações do usuário
	glutMainLoop();

	return 0;
}

// Função callback de redesenho da 
// janela de visualização
void Desenha(void)
{
	// Limpa a janela de visualização com 
	// a cor de fundo definida previamente
	glClear(GL_COLOR_BUFFER_BIT);

	// funcao que desenha eixos cartesianos 
	// Note que esses eixos são apenas 
	// uma normalização que vc desenha, 
	// não é o tamanho da janela windows
	DesenhaEixos();

	switch (CURVA)
	{
	case Curva::BEZIER:
		DesenhaBezierGrau3(); break;
	case Curva::HERMITE:
		DesenhaHermiteGrau3(); break;
	case Curva::PARABOLA:
		DesenhaParabola(); break;
	default: break;
	}

	// Executa os comandos OpenGL 
	glFlush();
}

// Esta função não é CallBack de desenho
// ela é chamada pela callback Desenha 
// apenas para separar a tarefa de 
// desenhar das outras tarefas;
// A tarefa aqui é desenhar os 
// eixos cartesianos
// Importante notar que as dimenções 
// dos eixos cartezianos nao são em pixels, 
// como definido no main em 
// glutInitWindowSize
// A escala desses eixos é definida na 
// função void AlteraTamanhoJanela, nas 
// linhas gluOrtho2D...
// é uma questão de organização de codigo
// O programa pode funcionar sem ela
void DesenhaEixos()
{
	// EIXOS

	// Desenha a linha horizontal
	glColor3f(0.0, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex2f(MENORX, 0.0f);
	glVertex2f(MAIORX, 0.0f);
	glEnd();

	// Desenha a linha vertical
	glColor3f(0.0, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex2f(0.0f, MENORY);
	glVertex2f(0.0f, MAIORY);
	glEnd();
}

// Essa função desenha a curva de Hermite 
// de Grau 3. 
// A matriz de Hermite já está colocada
// no código pra vc, 
// é a variável H. 
// E os pontos de Hermite são 
// fornecidos pela interface 
// clicando com o mouse. 
void DesenhaHermiteGrau3()
{
	// os pontos são guardados, 
	// juntamente com 
	// os ângulos e tangentes,
	// que devem ser fornecidos 
	// diretamente no código
	if (GET_POINTS
		&& (TOTAL_POINTS < 2))
	{
		if ((TOTAL_POINTS == 0)
			|| ((PH[TOTAL_POINTS - 1][0] != bx)
				&& (PH[TOTAL_POINTS - 1][1] != by)))
		{
			// guarda coordenada x
			PH[TOTAL_POINTS][0] = bx;

			// guarda coordenada y
			PH[TOTAL_POINTS][1] = by;

			// tangente x do 
			// angulo de incidência de entrada
			PH[TOTAL_POINTS][2] = tang1x;

			// tangente y do 
			// angulo de incidência de entrada
			PH[TOTAL_POINTS][3] = tang1y;

			TOTAL_POINTS += 1;
			if (TOTAL_POINTS > 2)
				TOTAL_POINTS = 2;
		}
	}

	// exibe os 2  pontos de controle
	glColor3f(1.0, 0.0f, 0.0f);
	glPointSize(5.0f);
	for (int i = 0; i < TOTAL_POINTS; i++)
	{
		glBegin(GL_POINTS);
	    glVertex2f(PH[i][0], PH[i][1]);
		glEnd();
	}

	// agora que vc tem 
	// os dois pontos e 
	// as duas tangentes, 
	// que estão em P
	// basta só utilizar a teoria 
	// vista nos vídeos para montar 
	// o polinomio de Hermite 
	// e plotar a curva. 
	// Não se esqueça de variar 
	// a variável t de 0 a 1, 
	// indo de 0.1 em 0.1, pelo menos, 
	// ou 0.01 em 0.01. 
	if (TOTAL_POINTS == 2)
	{
		glColor3f(1.0, 0.0f, 0.0f);

		// Matriz de Hermite de Grau 3
		GLfloat H[4][4] =
		{
			{  2.0f, -2.0f,  1.0f,  1.0f },
			{ -3.0f,  3.0f, -2.0f, -1.0f },
			{  0.0f,  0.0f,  1.0f,  0.0f },
			{  1.0f,  0.0f,  0.0f,  0.0f }
		};

		GLfloat X[] = {
			PH[0][0],
			PH[1][0],
			tang1x,
			tang2x
		};

		GLfloat Y[] = {
			PH[0][1],
			PH[1][1],
			tang1y,
			tang2y
		};

		GLfloat passo = 0.01;

		for (GLfloat t = 0;
			t < 1;
			t += passo)
		{
			GLfloat T0[] = {
				pow(t, 3),
				pow(t, 2),
				t,
				1
			};
			GLfloat T1[] = {
				pow(t + passo, 3),
				pow(t + passo, 2),
				t + passo,
				1
			};

			// aqui calcula cada trecho da curva
			GLfloat xini = multiply(T0,
				H, X);
			GLfloat yini = multiply(T0,
				H, Y);

			GLfloat xfin = multiply(T1,
				H, X);
			GLfloat yfin = multiply(T1,
				H, Y);

			// esse trecho plota cada pedaço da curva
			// entre seus pontos
			glBegin(GL_LINE_STRIP);
			glVertex2f(xini, yini);
			glVertex2f(xfin, yfin);
			glEnd();
		}
	}
}

void DesenhaBezierGrau3()
{
	// GET_POINTS = Status de 
	// atribuição de pontos pelo mouse
	// esse segundo trecho salva 
	// os pontos clicados 
	// da curva de Bezier 
	if (GET_POINTS
		&& (TOTAL_POINTS < 4))
	{
		// testa se o ponto aqul clicado
		// é o primeiro ou 
		// é diferente do anterior
		if ((TOTAL_POINTS == 0)
			|| ((PB[TOTAL_POINTS - 1][0] != bx)
				&& (PB[TOTAL_POINTS - 1][1] != by)))
		{
			PB[TOTAL_POINTS][0] = bx;
			PB[TOTAL_POINTS][1] = by;

			TOTAL_POINTS += 1;
			if (TOTAL_POINTS > 4)
				TOTAL_POINTS = 4;
		}
	}

	// exibe apenas os 4  pontos de controle
	glColor3f(1.0, 0.0f, 0.0f);
	glPointSize(5.0f);

	for (int i = 0; i < TOTAL_POINTS; i++)
	{
		glBegin(GL_POINTS);
		glVertex2f(PB[i][0], PB[i][1]);
		glEnd();
	}

	// nesse trecho de codigo, 
	// vc deve calcular todos os pontos 
	// (nao de controles)
	// por onde passa a curva de Bezier. 
	// Para isso, tem que saber a teoria
	if (TOTAL_POINTS == 4)
	{
		// Matriz de Bézier de Grau 3
		GLfloat B[4][4] = {
			{ -1.0f,  3.0f, -3.0f,  1.0f },
			{  3.0f, -6.0f,  3.0f,  0.0f },
			{ -3.0f,  3.0f,  0.0f,  0.0f },
			{  1.0f,  0.0f,  0.0f,  0.0f }
		};

		GLfloat X[] = {
			PB[0][0],
			PB[1][0],
			PB[2][0],
			PB[3][0]
		};

		GLfloat Y[] = {
			PB[0][1],
			PB[1][1],
			PB[2][1],
			PB[3][1]
		};

		GLfloat passo = 0.01;

		glColor3f(1.0, 0.0f, 0.0f);


		for (GLfloat t = 0; t < 1; t += passo)
		{
			GLfloat T0[] = {
				pow(t, 3),
				pow(t, 2),
				t,
				1
			};
			GLfloat T1[] = {
				pow(t + passo, 3),
				pow(t + passo, 2),
				t + passo,
				1
			};

			// aqui calcula cada trecho da curva
			GLfloat xini = multiply(T0,
				B, X);
			GLfloat yini = multiply(T0,
				B, Y);

			GLfloat xfin = multiply(T1,
				B, X);
			GLfloat yfin = multiply(T1,
				B, Y);

			// esse trecho plota cada pedaço da curva
			// entre seus pontos
			glBegin(GL_LINE_STRIP);
			glVertex2f(xini, yini);
			glVertex2f(xfin, yfin);
			glEnd();
		}
	}
}

void DesenhaParabola()
{
	//y = ax^2 + bx + c
	// parâmetros
	// grau
	GLfloat n = 2;
	// discretização
	GLfloat passo = 0.01;
	// constante de x^2
	GLfloat a = 0.03;
	// constante de x
	GLfloat b = 0;
	// constante isolada
	GLfloat c = 20;

	glColor3f(1.0, 0.0f, 0.0f);

	// plota os demais pontos
	for (GLfloat xini = -MENORX;
		xini < MAIORX;
		xini += passo)
	{
		// aqui calcula cada 
		// ponto da parábola
		GLfloat yini = a * pow(xini, 2)
			+ b * xini
			+ c;

		GLfloat xfin = xini + passo;
		GLfloat yfin = a * pow(xfin, 2)
			+ b * xfin
			+ c;

		// esse trecho plota cada 
		// pedaço da parábola
		// entre seus pontos
		glBegin(GL_LINE_STRIP);
		glVertex2f(xini, yini);
		glVertex2f(xfin, yfin);
		glEnd();
	}
}

// Função callback chamada quando 
// o tamanho da janela é alterado 
void AlteraTamanhoJanela(GLsizei w,
	GLsizei h)
{
	// Evita a divisao por zero
	if (h == 0) h = 1;

	// Especifica as dimensões da Viewport
	glViewport(0, 0, w, h);

	// Atualiza as variáveis
	GLfloat largura = w,
		altura = h;

	// Inicializa o sistema de coordenadas
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Estabelece a janela de seleção 
	// (esquerda, direita, 
	//  inferior, superior) 
	// mantendo a proporção com 
	// a janela de visualização
	if (largura <= altura)
	{
		GLdouble scale_y = altura / largura;
		gluOrtho2D(MENORX,
			MAIORX,
			MENORY * scale_y,
			MAIORY * scale_y);
	}
	else
	{
		GLdouble scale_x = largura / altura;
		gluOrtho2D(MENORX * scale_x,
			MAIORX * scale_x,
			MENORY,
			MAIORY);
	}
}

// Função callback chamada para 
// gerenciar eventos de teclas
void Teclado(unsigned char key,
	int x, int y)
{
	// sai comm ESC
	if (key == 27)
		exit(0);
}

// Função responsável por 
// inicializar parâmetros e variáveis
void Inicializa(void)
{
	// Define a cor de fundo da 
	// janela de visualização como branca
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void myMouseFuncIterate(int button,
	int state, int x, int y)
{
	if (state == GLUT_UP)
	{
		switch (button) {
		case GLUT_LEFT_BUTTON:
		{
			bx = x;
			by = 400 - y;
			GET_POINTS = 1;
			break;
		}
		case GLUT_RIGHT_BUTTON: {

			GET_POINTS = 0;
			TOTAL_POINTS = 0;
			break;
		}
		default: break;
		};
	}

	glutPostRedisplay();
}

// função que multiplica as 
// 3 matrizes
GLfloat multiply(GLfloat T[],
	GLfloat M[][4], GLfloat P[])
{
	GLfloat MP[4];

	// m ultiplica primeiro H por M
	for (int i = 0; i < 4; i++)
	{
		MP[i] = 0;
		for (int j = 0; j < 4; j++)
		{
			MP[i] = MP[i] + M[i][j] * P[j];
		}
	}

	// multiplica T * HM
	GLfloat R = 0;
	for (int i = 0; i < 4; i++)
	{
		R = R + T[i] * MP[i];
	}

	return R;
}
