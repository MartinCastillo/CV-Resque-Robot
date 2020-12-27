/*
 Funciones que hacen cosas muy acotadas y abstractas, por ejemplo chequear si un array contiene un elemento
 */
 //Función que robé de internet para ver si hay cierto elemento en un array
boolean arrayIncludeElement(int _array[n_sensores], int element) {
  for (int i = 0; i < n_sensores; i++) {
    if (_array[i] == element) {
      return true;
    }
  }
  return false;
}

void imprime_lista(int list[n_sensores], String add) {
  //Esta función imprime la lista de parametro de forma ordenada
  // tiene un parametro adicional que es el add
  String B = "|";
  //Imprime los valores de una lista de forma ordenada
  for (int i = 0; i < n_sensores; i++) {
    B += (String(list[i]) + " | ");
  }
  Serial.println(B + " || " + add);
}
