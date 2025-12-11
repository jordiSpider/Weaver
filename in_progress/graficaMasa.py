import argparse
import matplotlib.pyplot as plt

def leer_datos(ruta_archivo):
    try:
        with open(ruta_archivo, 'r') as archivo:
            # Leer todas las líneas del archivo
            lineas = archivo.readlines()

        # La primera línea contiene los nombres de las columnas
        encabezados = lineas[0].strip().split('\t')

        # Inicializar el diccionario para almacenar los datos
        datos = {col: [] for col in encabezados}
        
        # Leer los datos de las filas restantes
        for linea in lineas[1:]:
            valores = linea.strip().split('\t')
            for col, val in zip(encabezados, valores):
                datos[col].append(float(val))

        return datos
    except Exception as e:
        print(f"Error al leer el archivo: {e}")
        return {}

def graficar_datos(datos):
    if not datos:
        print("No hay datos para graficar.")
        return

    # La primera columna es el eje x
    x_vals = datos.pop(next(iter(datos)))

    # Determinar las columnas para la primera y segunda gráfica
    columnas = list(datos.keys())
    columnas_primera_grafica = columnas[:-3]
    columnas_segunda_grafica = columnas[-3:]

    fig, axs = plt.subplots(1, 2, figsize=(14,6))

    # Graficar la primera gráfica
    for nombre_serie in columnas_primera_grafica:
        y_vals = datos[nombre_serie]
        axs[0].plot(x_vals, y_vals, marker='.', label=nombre_serie)
    axs[0].set_xlabel('Eje X')
    axs[0].set_ylabel('Eje Y')
    axs[0].set_title('Primera gráfica')
    axs[0].legend()
    axs[0].grid(True)

    # Graficar la segunda gráfica
    for nombre_serie in columnas_segunda_grafica:
        y_vals = datos[nombre_serie]
        axs[1].plot(x_vals, y_vals, marker='.', label=nombre_serie)
    axs[1].set_xlabel('Eje X')
    axs[1].set_ylabel('Eje Y')
    axs[1].set_title('Segunda gráfica')
    axs[1].legend()
    axs[1].grid(True)
    
    
    plt.tight_layout()
    plt.show()

def main():
    parser = argparse.ArgumentParser(description='Leer y graficar datos de un archivo txt tabulado.')
    parser.add_argument('ruta_archivo', type=str, help='Ruta al archivo txt con los datos.')
    args = parser.parse_args()

    datos = leer_datos(args.ruta_archivo)
    graficar_datos(datos)


main()