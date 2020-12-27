/*
 Funciones que se encarfgan de procesar arrays de la información ya obtenida de la línea
 */
float get_line_pos(int _linea[n_sensores], int buscar) {
  //Promedio de valores de puntos negro, para segír la línea
  //Busca los 1 (ahora los 'buscar')
  //ACTUALIZACIÖN: Promedia los valores con el valor buscar

  float ret = 0; float n_negros = 0; //Número de sensores en línea
  if (arrayIncludeElement(_linea, buscar)) { //Si hay negros en la línea
    for (int i = 0; i < n_sensores; i++) {
      if (_linea[i] == buscar) {
        n_negros += 1;
        ret += i;
      }
    }
    return (ret / n_negros);
  }
  else {
    return -1;
  }
}

//Evalua si es línea, rojo o blanco
bool evaluar_sensores(int _linea[n_sensores], int _ret_sensores[n_sensores]) {
  /*Toma los valores de salida de los sensores y dice si hay línea, rojo o blanco
    Esto según las variables de valor de colores, por lo que también toma los valores
    del array linea, que se pasa por referencia*/
  //Como valores posibles estan: 1 para negro, 0 para blanco y -1 para rojo
  for (int i = 0; i < n_sensores; i++) {
    int valor = _ret_sensores[i]; //Valor del sensor actual
    int v_rojo = v_rojos[i];
    int v_blanco  = v_blancos[i];
    int v_negro = v_negros[i];
    if ((valor >= v_rojo - umbral) && (valor < v_rojo + umbral)) { //Si es rojo
      _linea[i] = -1;
    }
    else if (valor >= v_rojo + umbral) { //Si es negro
      _linea[i] = 1;
    }
    else if (valor < v_rojo - umbral) { //Si no es blanco
      _linea[i] = 0;
    }
  }
  return true;
}

bool leer_sensores(int _ret_sensores[n_sensores], int calibrar) {
  //Cambia el valor del array de los valores de los sensores según el sensor que deba,
  //El segundo parametro es si actualiza o no según la lista de desviaciónes del promedio
  //Por cada sensor
  for (int i = 0; i < n_sensores; i++) {
    int valor_ajuste = (calibrar == 1) ? constantes_calibrar[i] : 0;
    _ret_sensores[i] = analogRead(sensores[i]) + valor_ajuste;
  }
  return true;
}
