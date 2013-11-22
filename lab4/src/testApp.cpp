#include "testApp.h"

// --------------------------------------------------------------
// Funciones para el manejo de la estructura

// Se limpia la estructura y se inicializan las variables
void testApp::inicializar(){
	for(int i = 0; i < cantInstrumentos; i++){
		for(int j = 0; j < largoMaxPista; j++){
			// Las notas de la primera vuelta van en 0
			if (j < vuelta)
				pistas[i][j].tipo = 0;
			else
				pistas[i][j].tipo = -1;
			pistas[i][j].nota = -1;
			//pistas[i][j].velocidad = -1;
		}
		posSiguienteSonido[i] = 0;
		vueltasPista[i] = 1;
	}
	numPista = -1;
	numPistaBack = -1;
	enviar = false;
	dejarDeMostrar = true;
	sequencer = false;
	tonalidad = 0;
	posActual = 0;
	canales = 1;
	velocidad = 1;
	bpm = 60;
	mandeLed = false;

	// Inicialización variables Arduino
	inicializarVectorArduino();
}

// Reproduce el tiempo actual de todos los intrumentos
void testApp::reproducirTiempo(int tiempo){
	for(int i = 0; i < cantInstrumentos; i++){
		midiOut[i].sendNoteOn(canales, pistas[i][tiempo].nota, velocidad);
	}
	Sleep(500);
	for(int i = 0; i < cantInstrumentos; i++){
		midiOut[i].sendNoteOff(canales, pistas[i][tiempo].nota, velocidad);
	}
}

// Comienza a reproducir un tiempo, NO lo finaliza, osea no manda noteOFF
void testApp::comenzarReproduccionTiempo(int tiempo){
	numPistaBack = numPista;
	for(int i = 0; i < cantInstrumentos; i++){
		// Si es el actual entonces no hace bucle
		if (i == numPistaBack){
			// Si tiene sonido
			if (pistas[i][tiempo].tipo == 1)
				midiOut[i].sendNoteOn(canales, pistas[i][tiempo].nota, velocidad);
		}
		// Si tiene nota en este tiempo reproduzco directo, sino me fijo  y reproduzco según eso
		else{
			if (pistas[i][tiempo].tipo == 1){
				midiOut[i].sendNoteOn(canales, pistas[i][tiempo].nota, velocidad);
			}// Si no tiene nota
			else if (pistas[i][tiempo].nota == -1){
				// se obtiene cuantos tiempos tiene esta pista
				int tiempoTotal = vuelta * vueltasPista[i];
				int posReproducir = tiempo % tiempoTotal;
				// se hace q la vuelta actual calze con las que realmente tiene, osea se hace loop
				if (pistas[i][posReproducir].tipo == 1)
					midiOut[i].sendNoteOn(canales, pistas[i][posReproducir].nota, velocidad);
			}
		}
	}
}

// Finaliza la reproducción de un tiempo
void testApp::finalizarReproduccionTiempo(int tiempo){
	for(int i = 0; i < cantInstrumentos; i++){
		// SI es el actual entonces no se hizo bucle
		if (i == numPistaBack){
			// Si tenia sonido
			if (pistas[i][tiempo].tipo == 1)
				midiOut[i].sendNoteOff(canales, pistas[i][tiempo].nota, velocidad);
		}
		// SI tiene nota en este tiempo reproduzco directo, sino me fijo  y detengo según eso
		else{
			if (pistas[i][tiempo].tipo == 1){
				midiOut[i].sendNoteOff(canales, pistas[i][tiempo].nota, velocidad);
			}
			else if (pistas[i][tiempo].nota == -1){
				// se obtiene cuantos tiempos tiene esta pista
				int tiempoTotal = vuelta * vueltasPista[i];
				int posReproducir = tiempo % tiempoTotal;
				// se hace q la vuelta actual calze con las que realmente tiene, osea se hace loop
				if (pistas[i][posReproducir].tipo == 1)
					midiOut[i].sendNoteOff(canales, pistas[i][posReproducir].nota, velocidad);
			}
		}
	}
	numPistaBack = numPista;
}

// Agrega un tiempo en el instrumento actual
void testApp::agregarVuelta(int p){
	// busco en que tiempo esta la posSiguienteSonido[p]
	int posicion = (vueltasPista[p] * vuelta);
	for(int i = 0; i < vuelta; i++){
		pistas[p][posicion + i].tipo = 0;		
	}
	vueltasPista[p] = vueltasPista[p] + 1;
}

// Agrega un sonido a la estructura
void testApp::agregarSonido(int p, int t, int n){
	// Si se agrega a la misma pista que se pudo haber reproducido antes y tenia sonido	-	-	-	-	-	-	-	ACAAAA
	if (p == numPistaBack){
		if (pistas[p][posActual].tipo == 1)
			midiOut[p].sendNoteOff(canales, pistas[p][posActual].nota, velocidad);
	}
	else{	// Si era otra pista y tenia sonido
		if (pistas[numPistaBack][posActual].tipo == 1){
			midiOut[numPistaBack].sendNoteOff(canales, pistas[numPistaBack][posActual].nota, velocidad);
		}	// Si era otra pista pero estaba en bucle
		else if (pistas[numPistaBack][posActual].nota == -1){
			// se obtiene cuantos tiempos tiene esta pista
			int tiempoTotal = vuelta * vueltasPista[numPistaBack];
			int posReproducir = posActual % tiempoTotal;
			// se hace q la vuelta actual calze con las que realmente tiene, osea se hace loop
			if (pistas[numPistaBack][posReproducir].tipo == 1)
				midiOut[numPistaBack].sendNoteOff(canales, pistas[numPistaBack][posReproducir].nota, velocidad);
		}
	}
	pistas[p][posActual].tipo = t;
	pistas[p][posActual].nota = n;
	// mantengo esta actualización para la función "quedaSonido"
	posSiguienteSonido[p] = posActual + 1;
	midiOut[p].sendNoteOn(canales, pistas[p][posActual].nota, velocidad);
}
// --------------------------------------------------------------

// Funcion auxiliar para ver si queda sonido por reproducir
bool testApp::quedaSonido(int pos){
	int i = 0;
	// Mientras me falte revisar un instrumento y el actual no tenga para reproducir
	while ((i < cantInstrumentos) && ((vueltasPista[i] * vuelta) <= pos)){
		i++;
	}
	// No importa el tiempo porque terminaria la canción si ninguno tiene nada mas para reproducir
	if ((i < cantInstrumentos) || (pos % vuelta != 0))
		return true;
	else
		return false;
}

// Funcion para enviar todos los sonidos al Ableton
stringstream testApp::enviarSonidos(){
	int tiempo = 500 + bpm; //HACER ALGO ACA CON LOS BPM
	int pos = 0;
	stringstream text;
	text << "Fue enviado: " << endl;
	// para el tema de repetir en vueltas cuando uno no tiene mas sonidos
	int posSonido[cantInstrumentos];
	for(int i = 0; i < cantInstrumentos; i++){
		posSonido[i] = 0;
	}
	while(quedaSonido(pos)){
		if (pos % vuelta == 0)
			text << "Vuelta " << pos / vuelta << endl;
		for(int i = 0; i < cantInstrumentos; i++){
			// Si no tiene mas sonidos entonces empieza desde el principio este instrumento (porque hay otro que si tiene)
			if (pistas[i][posSonido[i]].tipo == -1)
				posSonido[i] = 0;
			if (pistas[i][posSonido[i]].tipo == 1)
				midiOut[i].sendNoteOn(canales, pistas[i][posSonido[i]].nota, velocidad);
			text << nombre[i] << " [" << pistas[i][posSonido[i]].tipo << " " << pistas[i][posSonido[i]].nota << "] - ";
			posSonido[i]++;
		}
		text << endl;
		ofDrawBitmapString(text.str(), 20, 20);
		Sleep(tiempo);
		// Se manda noteOff
		for(int i = 0; i < cantInstrumentos; i++){
			if (pistas[i][posSonido[i]-1].tipo == 1)
				midiOut[i].sendNoteOff(canales, pistas[i][posSonido[i]-1].nota, velocidad);
		}
		pos++;
	}
	return text;
}

// ----------------- ARDUINO ---------------------
// Funcion de inicializacion de elementos del arduino
void testApp::inicializarVectorArduino(){
	valor1 = 0;
	sA0 = 0;
	sA1 = 0;
	sA2 = 0;

	valMax[22] = 130;
    valMin[22] = 100;
    valMax[21] = 130;
    valMin[21] = 100;
    valMax[20] = 115;
    valMin[20] = 85;
    valMax[19] = 115;
    valMin[19] = 75;
	valMax[18] = 115;
    valMin[18] = 70;
	valMax[17] = 120;
    valMin[17] = 90;

    //notas
	valMax[8] = 120;
    valMin[8] = 80;
	valMax[7] = 150;
    valMin[7] = 70;
    valMax[6] = 250;
    valMin[6] = 60;
    valMax[5] = 250;
    valMin[5] = 90;
    valMax[4] = 250;
    valMin[4] = 60;
    valMax[3] = 250;
    valMin[3] = 60;
    valMax[2] = 250;
    valMin[2] = 60;

	for(int i = 0;i<cantDisp;i++){
		cantMenores[i] = 0;
	}
}

// Conecta con los dos arduinos
void testApp::conectarArduinos(){
	SP1 = new Serial("\\\\.\\COM19");    // poner el adecuado - el de las notas
	if (SP1->IsConnected())
		printf("Arduino 1 conectado\n");
	SP2 = new Serial("\\\\.\\COM20");    // poner el adecuado - el de los isntrumentos
	if (SP2->IsConnected())
		printf("Arduino 2 conectado\n");

	// resetear led
	SP2->WriteData("S",2);
	SP2->WriteData("S",2);
}

// Funcion de atencion al Arduino
void testApp::atenderDisp(int n){
	// Se identifica si es instrumento o nota
	if ((n >= 17) && (n <= 22)){
		// Es instrumento
		if (numPista == (n - 17))
			numPista = -1;
		else{
			if (n == 17){// Oboe
				numPista = 3;	
			}
			else if (n == 18){ // Flauta
				numPista = 1;
			}
			else if (n == 19){ // Acordeon
				numPista = 2;
			}
			else if (n == 20){ // guitarra
				numPista = 0;
			}
			else if (n == 21){ // piano
				numPista = 4;
			}
			else if (n == 22){ // Tambor
				numPista = 5;
			}
		}
			

		// Cambiar estado LED
		if (numPista == 0)	//guitarra
			SP2->WriteData("S",2);
		else if (numPista == 1) // flauta
			SP2->WriteData("J",2);
		else if (numPista == 2) // Acordeón
			SP2->WriteData("H",2);
		else if (numPista == 3) //Oboe
			SP2->WriteData("L",2);
		else if (numPista == 4) //Piano
			SP2->WriteData("K",2);
		else if (numPista == 5)	//tambor
			SP2->WriteData("I",2);
		mandeLed = true;
		///	0 = Guitarra
		/// 1 = Flauta
		/// 2 = Acordeón
		/// 3 = Oboe
		/// 4 = Piano
		/// 5 = Tambor
		// empieza de nuevo la secuencia
		posActual = -1;
	}
	else if ((n >= 2) && (n <= 10)){
		// Es nota
		int nota = 0;
		if (n == 2)
			nota = 30 + tonalidad;
		else if (n == 3)
			nota = 32 + tonalidad;
		else if (n == 4)
			nota = 34 + tonalidad;
		else if (n == 5)
			nota = 35 + tonalidad;
		else if (n == 10)
			nota = 37 + tonalidad;
		else if (n == 9)
			nota = 39 + tonalidad;
		else if (n == 8)
			nota = 41 + tonalidad;
		if (numPista != -1)
			agregarSonido(numPista,1,nota);
	}
}

// Actualiza el valor del pin de numero disp
bool testApp::actualizarMenores(int v, int disp, int arduino){ // 1 = Arduino1, 2 = Arduino2
	if (arduino == 1){
		if (v > 1){
			if(cantMenores[disp]<cantPromedio)
				cantMenores[disp]++;
		}else{
			if(cantMenores[disp]>0)
				cantMenores[disp]--;
		}
		if(cantMenores[disp] >= cantPromedio-3)
			return true;
		else
			return false;
	}
	else{
		if(disp == 9 || disp == 11){
			if (v == 128){
				if(cantMenores[disp]<6)
					cantMenores[disp]++;
			}else{
				if(cantMenores[disp]>0)
					cantMenores[disp]--;
			}
			if(cantMenores[disp] >= 6)
				return true;
			else
				return false;
		}
		else{	
			if (v == 128){
				if(cantMenores[disp]<cantPromedio)
					cantMenores[disp]++;
			}else{
				if(cantMenores[disp]>0)
					cantMenores[disp]--;
			}
			if(cantMenores[disp] >= cantPromedio-2)
				return true;
			else
				return false;
		}
	}
}

// Toma el valor que le llega de los datos de arduino 1
int testApp::extraigoValorA1(int i,char* datos){
	char num [256] ="0";
	char num1 [256] ="0";
	char num2 [256] ="0";
	char num3 [256] ="0";

	int p = 0;
	//descarto el numero de dispositivo
	while((p < 256) && (datos[p] != '\n')){
		p++;
	}
	p++;//descarto el \n
	int j=0;
	while((p < 256) &&(datos[p] != '-')){
		num[j]=datos[p];
		p++;
		j++;
	}
	if (p < 256)
		num[j] = '\n';
	int r = atoi(num);
	//obtengo los valores analogicos
	j = 0;
	p++;
	while((p < 256) &&(datos[p] != '-')){
		num1[j]=datos[p];
		p++;
		j++;
	}
	sA0 = atoi(num1);
	j = 0;
	p++;
	while((p < 256) &&(datos[p] != '-')){
		num2[j]=datos[p];
		p++;
		j++;
	}
	sA1 = atoi(num2);
	j = 0;
	p++;
	while((p < 256) &&(datos[p] != '\n')){
		num3[j]=datos[p];
		p++;
		j++;
	}
	sA2 = atoi(num3);

	if (p < 256)
		return r;
	else
		return -1;
}

// Toma el valor que le llega de los datos de arduino 1
int testApp::extraigoValorA2(int i,char* datos){
	char num [256] ="0";
	int p = 0;
	//descarto el numero de dispositivo
	while((p < 256) && (datos[p] != '\n')){
		p++;
	}
	p++;//descarto el \n
	int j=0;
	while((p < 256) &&(datos[p] != '\n')){
		num[j]=datos[p];
		p++;
		j++;
	}
	if (p < 256)
		num[j] = '\n';
	int r = atoi(num);
	if (p < 256)
		return r;
	else
		return -1;
}

//--------------------------------------------------------------
void testApp::setup(){
	ofSetVerticalSync(true);
	ofBackground(255);
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	// Se muestra la lista de puertos midi que hay. Solo para comprobar
	midiOut[0].listPorts(); 
	
	// Conecto los puertos midi
	/// INSTRUMENTOS
	///	0 = Guitarra
	/// 1 = Flauta
	/// 2 = Acordeón
	/// 3 = Oboe
	/// 4 = Piano
	/// 5 = Tambor
	// Se abre cada puerto
	nombre[0] = "Guitarra";
	nombre[1] = "Flauta";
	nombre[2] = "Acordeon";
	nombre[3] = "Oboe";
	nombre[4] = "Piano";
	nombre[5] = "Tambor";
	for(int i = 0; i < cantInstrumentos; i++){
		midiOut[i].openPort(nombre[i]);
	}
	
	inicializar();

	conectarArduinos();
}

//--------------------------------------------------------------
void testApp::update() {
	int iteraciones = 0;
	// cada nota es 1 segundo, 
	comenzarReproduccionTiempo(posActual);
	int tiempoMin = 70;
	// Se calcula cuanto demora la nota segun los bpm
	int tiempo = 60000 / (bpm*tiempoMin);
	while (iteraciones < tiempo)
	{
		if(SP2->IsConnected()){//si esta conectado
			//	Se escucha y recibe de arduino
			char data [256] = "";
			int x = SP2->ReadData(data,256);	//leo del arduino
			//	Se decodifica mensaje arduino
				if(x!=-1){
					//printf ("A2 %s \n",data);
					int pin;
					if (data[1]!='-'){
							char aux [4] ="";
							aux[0]=data[0];
							aux[1]=data[1];
							aux[2]='\n';
							pin = atoi(aux);
					}
					else{
						pin = ((int)data[0]) - 48;
					}
					valor1 = extraigoValorA2(x,data);
					if((valor1 != -1) && (actualizarMenores(valor1,pin+10, 2))){
						printf ("Tocaron el pin: %d A2\n",pin);
						//	Se procesa mensaje arduino
						atenderDisp(pin+10);
					}
				}
				if (!mandeLed)
					SP2->WriteData("h",2);
				else
					mandeLed = false;
		}
		// Creo q es el arduino de las notas
		if(SP1->IsConnected()){//si esta conectado
			//	Se escucha y recibe de arduino
				char data [256] = "";
				int x = SP1->ReadData(data,256);	//leo del arduino
				//	Se decodifica mensaje arduino
				if(x!=-1){
					//printf ("A1 %s \n",data);
					int pin;
					// extraigo numero de pin
					if (data[1]!='-'){
							char aux [4] ="";
							aux[0]=data[0];
							aux[1]=data[1];
							aux[2]='\n';
							pin = atoi(aux);
					}
					else{
						pin = ((int)data[0]) - 48;
					}
					valor1 = extraigoValorA1(x,data);
					if((valor1 != -1) && (actualizarMenores(valor1,pin, 1))){
						printf ("Tocaron el pin: %d A1\n",pin);					
						//	Se procesa mensaje arduino
						atenderDisp(pin);
					}		
					if(sA0==1023){
						tonalidad = -12;
					}
					if(sA1==1023){
						tonalidad = 0;
					}
					if(sA2==1023){
						tonalidad = 12;
					}
				}
				SP1->WriteData("h",2);
		}
		Sleep(tiempoMin);
		iteraciones++;
	}
	// finaliza el tiempo, corta el sonido
	finalizarReproduccionTiempo(posActual);
	// Si queda algo para reproducir sigue, sino vuelve a cero
	posActual++;
	if (numPista != -1){
		if (pistas[numPista][posActual].tipo == -1){
			agregarVuelta(numPista);
		}
	}
	else{
		if (!quedaSonido(posActual)){
			posActual = 0;
		}
	}
}

//--------------------------------------------------------------
void testApp::draw() {
	if(enviar){
		ofSetColor(0);
		aImprimir = enviarSonidos();
		if (!sequencer)
			enviar = false;
		dejarDeMostrar = false;
		delete SP1;
		delete SP2;
	}
	else{
		if (dejarDeMostrar){
			// muestro info de lo que hay guardado
			ofSetColor(0);
			stringstream text;
			//text << "Notas: a - j. Limpiar: l. Enviar al ableton: e. Silencio k. Dejar de mostrar info mandada q. Secuencer on/off p.";
			text << "Notas: a - j. Limpiar: l. Agregar tiempo: t. Arriba y abajo cambiar pista. Derecha e izquierda cambiar tiempo."; 
			//Enviar al ableton: e. Silencio k. Dejar de mostrar info mandada q. Secuencer on/off p.";
			text << endl << "BPM: " << bpm << ", tiempo actual: " << posActual << ", Tonalidad: " << tonalidad << endl ;
			for(int i = 0; i < cantInstrumentos; i++){
				if (numPista == i)
					text << "[*]";
				else
					text << "[ ]";
				text << nombre[i] << endl;
				for(int j = 0; j < largoMaxPista; j++){
					text << " " << pistas[i][j].nota << " ";
					if (((j % vuelta) == (vuelta -1)) && (j != 0)){
						text << " == ";
					}
				}	
				text << endl;
				text << endl;
			}
			ofDrawBitmapString(text.str(), 20, 20);
		}
		else
		{
			ofDrawBitmapString(aImprimir.str(), 20, 20);
		}
	}
}

//--------------------------------------------------------------
void testApp::exit() {	
	// cierro puertos
	for(int i = 0; i < cantInstrumentos; i++){
		midiOut[i].closePort();
	}
}

//--------------------------------------------------------------
void testApp::keyPressed(int key) {
	switch (key){
		case OF_KEY_RIGHT:
			bpm = bpm++;
			break;
		case OF_KEY_LEFT:
			bpm = bpm--;
			if ( bpm < 0)
				bpm = 0;
			break;
		case OF_KEY_DOWN:
			if (numPista != 5)
				numPista++;
			else
				numPista = 0;
			posActual = -1;
			break;
		case OF_KEY_UP:
			if (numPista != 0)
				numPista--;
			else
				numPista = 5;
			posActual = -1;
			break;
		case 'e':
			enviar = !enviar;
			break;
		case 't':
			agregarVuelta(numPista);
			break;
		case 'l':
			inicializar();
			break;
		case 'q':
			dejarDeMostrar = true;
			conectarArduinos();
			break;
		case 'p':
			//sequencer = !sequencer;
			if (numPista == -1)
				numPista = 0;
			else
				numPista = -1;
			break;
		case 'a':
		case 's':
		case 'd':
		case 'f':
		case 'g':
		case 'h':
		case 'j':
		case 'k':
			// Mientras no haya arduino
			// si es letra o numero guardo nota
			/// MAPEO
			/// A = DO = 0;
			/// S = RE = 1;
			/// D = MI = 2;
			/// F = FA = 3;
			/// G = SOL = 4;
			/// H = LA = 5;
			/// I = SI = 6;
			int n;
			
			// key es el valor de la tecla presionada, el 0 a 6 es el rango de posibles valores de entrada y el 0 al 127 de salida
			switch(key){
				case 'a': // 55 comienzo
					n = 44 + tonalidad;
					break;
				case 's':
					n = 46 + tonalidad;
					break;
				case 'd':
					n = 48 + tonalidad;
					break;
				case 'f':
					n = 49 + tonalidad;
					break;
				case 'g':
					n = 51 + tonalidad;
					break;
				case 'h':
					n = 53 + tonalidad;
					break;
				case 'j':
					n = 55 + tonalidad;
					break;
				case 'k':
					n = -1;
					break;
			}
			if (numPista != -1){
				if (n == -1) // Silencio
					agregarSonido(numPista,0,-1);
				else
					agregarSonido(numPista,1,n);
			}
			break;
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key) {	
	
}