#include "ofMain.h"
#include "ofxMidi.h"
#include "SerialClass.h"	// Para conexion con Arduino

// Cantidad de instrumentos
const int cantInstrumentos = 6;
// Largo maximo de BPMs en una pista
const int largoMaxPista = 200;
// Largo de una vuelta
const int vuelta = 4;

// Constantes de comunicacion Arduino
const int cantDisp = 23; // 2 a 8 arduino1 = notas, 17 a 22 ardunio2 (se le suma 10 a lo que llega)
const int minValTouch = 60;
const int maxValTouch = 101;
const int cantPromedio = 5;


class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void exit();
	
	void keyPressed (int key);
	void keyReleased (int key);
	
	void inicializar();
	void reproducirTiempo(int tiempo);
	void comenzarReproduccionTiempo(int tiempo);
	void finalizarReproduccionTiempo(int tiempo);
	void agregarSonido(int p, int t, int n);
	void agregarVuelta(int p);

	bool quedaSonido(int pos);
	stringstream enviarSonidos();

	// MIDIs
	ofxMidiOut midiOut[cantInstrumentos];

	// Vamos a usar un solo canal y una sola velocidad
	int canales, velocidad;

	// Arreglo de nombres
	string nombre[cantInstrumentos];

	// Estructura donde se guardan los sonidos
	struct sonido{
		int tipo;	// -1 no tiene valor, 0 es silencio, 1 es sonido
		int nota;
	};
	
	// Matriz con todos los sonidos de las psitas
	struct sonido pistas[cantInstrumentos][largoMaxPista];

	// Arreglo con la posicion del siguiente sonido a ser insertado
	int posSiguienteSonido[cantInstrumentos];
	// Posicion de reproduccion actual
	int posActual;
	// Largo maximo actual de cada pista
	int vueltasPista[cantInstrumentos];

	// BPM
	float bpm;

	// Tonalidad 10 = aguda, 0 = normal, 10 = grave
	int tonalidad;

	int numPista;
	int numPistaBack;
	bool enviar;
	bool dejarDeMostrar;
	bool sequencer;
	stringstream aImprimir;


	// Comunicacion Arduino
	void atenderDisp(int n);
	void inicializarVectorArduino();
	void conectarArduinos();
	bool actualizarMenores(int v, int disp, int arduino);
	int extraigoValorA1(int i,char* datos);
	int extraigoValorA2(int i,char* datos);
	Serial* SP1;
	Serial* SP2;
	int valor1;
	int sA0;
	int sA1;
	int sA2;
	bool mandeLed;
	//matriz donde se guardan los ultimos 5 valores de cada dispositivo
	int cantMenores[cantDisp];
	int valMax [cantDisp];
	int valMin [cantDisp];
};
