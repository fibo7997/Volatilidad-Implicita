#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <cmath>
#include <limits>
#include <cstdint> // Incluido para std::uintmax_t
#include <boost/math/tools/minima.hpp> // libreria para ejecutar modelo de black scholes

using namespace std; // utilizado para usar las librerias sin el std::
using namespace boost::math::tools; // Para brent_find_minima

//divido los strings en substrings (tokens) con un loop  y obtengo un vector de tokens
vector<string> split(const string &s, char delim) {
    vector<string> tokens;
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

//para pasar de string a datetime en formato tm, "%m/%d/%Y %H:%M"
tm parseDateTime(const string& dateTime) {
    tm tm{};
    stringstream ss(dateTime);
    ss >> get_time(&tm, "%m/%d/%Y %H:%M");
    return tm;
}

//caclulo la funcion de la distribucion acumulada de un distribucion normal (para aplicar a bs despues)
//para valores positivos de x, la funcion usa una aproximacion polinomica. Los coeficientes (0.319381530, -0.356563782, etc.) estan predeterminados para fitear en un una funcion de distriubcion acumulada normal.
//cuando x es negativo, por simetria de la distriubion, tiene la misma probabilidad que x=1
double norm_cdf(const double x) {
    // Approximation of the normal CDF for positive x
    if (x >= 0.0) {
        double k = 1.0 / (1.0 + 0.2316419 * x);
        double k_sum = k * (0.319381530 + k * (-0.356563782 + k * (1.781477937 + k * (-1.821255978 + 1.330274429 * k))));
        return (1.0 - (1.0 / (pow(2 * M_PI, 0.5))) * exp(-0.5 * x * x) * k_sum);
    } else {
        return 1.0 - norm_cdf(-x);
    }
}

//calcula el precio de una opcion call europea usando el metodo de BS. d1 y d2 son calculados con el precio del subyacente, el strike, el time to maturity, la tasa libre de riesgo, y la volatilidad
//se mide la sensibilidad del precio del call a los diferentes parametros
double blackScholesCall(double S, double K, double T, double r, double sigma) {
    double d1 = (log(S / K) + (r + 0.5 * sigma * sigma) * T) / (sigma * sqrt(T));
    double d2 = d1 - sigma * sqrt(T);
    return S * norm_cdf(d1) - K * exp(-r * T) * norm_cdf(d2);
}

// clase creada para calcular la diferencia entre el precio teorico de un call usando BS, y su valor real de mercado (precioCall). Se utiliza para resolver la volatilidad implicita (siguiente paso)
//lass declaraciones en public pueden ser accedidas fuera de la clase, private no.
class BlackScholesPriceDiff {
public:
    BlackScholesPriceDiff(double S, double K, double T, double r, double marketPrice)
    : S(S), K(K), T(T), r(r), marketPrice(marketPrice) {}

    double operator()(double sigma) const {
        return std::abs(blackScholesCall(S, K, T, r, sigma) - marketPrice);
    }

private:
    double S;
    double K;
    double T;
    double r;
    double marketPrice;
};

//calcula la volatilidad implicita de un call europeo usando el metodo de Brent (algoritmo que busca minizmizar la diferencia entre el precio teorico(BS) y el precio de mercado)
//el objetivo es encontrar la volatilidad que hace que el precio del call calculado en BS matchee el precio de mercado (precioCall)
//los parametros podrian ser mas acotados ya que es dificil encontrar 1000% de VI. Las iteraciones aumentan las probabilidades de que el algoritmo encuentre un root.
double findImpliedVolatilityBoost(double S, double K, double T, double r, double marketPrice) {
    double low_vol = 0.003;
    double high_vol = 4.0;
    BlackScholesPriceDiff priceDiff(S, K, T, r, marketPrice);
    std::uintmax_t max_iterations = 100000; // Defino las max_iterations con standard uintmax_t

    std::pair<double, double> result = brent_find_minima(priceDiff, low_vol, high_vol, std::numeric_limits<double>::digits, max_iterations);

    return result.first; // El primer elemento es sigma (volatilidad)
}

//funcion principal
//primero abrimos el archivo csv.
int main() {
    ifstream file("C:\\Users\\Principal\\Desktop\\Neix\\Exp_Octubre.csv");
    if (!file.is_open()) {
        cerr << "Error" << endl;
        return 1;
    }

  //definimos csv de output
    ofstream outputFile("C:\\Users\\Principal\\Desktop\\Neix\\excel1.csv");
    if (!outputFile.is_open()) {
        cerr << "Error" << endl;
        file.close(); 
        return 1;
    }
//aca hacemos que el output file tenga un formato ( "." como separador decimal) para nuestros datos numericos
    outputFile.imbue(locale("C"));
//creo variable line de tipo string para leer linea por linea el archivo. utilizado con getline() mas adelante
    string line;
    
    // Esta es la fecha de vencimiento de los calls de octubre 2023 de Galicia. Usamos esta fecha para restarle cada linea del csv. y calcular el time to maturity.
    tm maturityDate = parseDateTime("10/21/2023 17:00");

    // Valor fijo de la tasa libre de riesgo
    const double risk_free_rate = 0.9;

    if (getline(file, line)) {
        outputFile << line << ";precio call;precio subyacente;Time to Maturity;Time to Maturity in Years;risk free rate;Implied Volatility" << endl;
    }

    while (getline(file, line)) {
        vector<string> tokens = split(line, ';');
        if (tokens.size() < 8) {
            cerr << "Skipping malformed line: " << line << endl;
            continue;
        }
    // extraemos los tokens y calculamos precioCall y precioSubyacente como el promedio de los valores provistos.
        string bid = tokens[3];
        string ask = tokens[4];
        string underBid = tokens[5];
        string underAsk = tokens[6];
        double bidValue = bid != "\\N" ? stod(bid) : 0.0;
        double askValue = ask != "\\N" ? stod(ask) : 0.0;
        double underBidValue = underBid != "\\N" ? stod(underBid) : 0.0;
        double underAskValue = underAsk != "\\N" ? stod(underAsk) : 0.0;
        double precioCall = (bidValue + askValue) / 2;
        double precioSubyacente = (underBidValue + underAskValue) / 2;
// parseamos la columna createAt en formato dateTime
        tm createdAt = parseDateTime(tokens[7]);

        // Convertimos a time_t para facilitar calculo 
        time_t createdAtTime = mktime(&createdAt);
        time_t maturityTime = mktime(&maturityDate);
        
        // Calculo al diferencia (para el time to maturity) en segundos, y despues convierto en dias, horas y minutos.
        double secondsDiff = difftime(maturityTime, createdAtTime);
        int days = secondsDiff / (24 * 3600);
        int hours = (int(secondsDiff) % (24 * 3600)) / 3600;
        int minutes = (int(secondsDiff) % 3600) / 60;

        // Convierto los dias, horas y minutos en fraccion de año tradeado (252 dias) 
        double timeToMaturityInYears = days / 252.0; // Solo dias
        timeToMaturityInYears += (hours / 24.0) / 252.0; // Agrego fraccion del dia proveniente de las horas
        timeToMaturityInYears += (minutes / 1440.0) / 252.0; // Lo mismo para los minutos

        //para formatear valores numericos como strings antes de convertirlo en objetos std::string
        stringstream precioCallStream;
        precioCallStream.imbue(locale::classic());
        precioCallStream << fixed << setprecision(2) << precioCall;
        string precioCallStr = precioCallStream.str();

        stringstream precioSubyacenteStream;
        precioSubyacenteStream.imbue(locale::classic());
        precioSubyacenteStream << fixed << setprecision(2) << precioSubyacente;
        string precioSubyacenteStr = precioSubyacenteStream.str();

        // Output con las nuevas columnas 
        outputFile << line << ";" 
                   << (bid != "\\N" && ask != "\\N" ? precioCallStr : "N/A") << ";" 
                   << (underBid != "\\N" && underAsk != "\\N" ? precioSubyacenteStr : "N/A") << ";"
                   << days << " days, " << hours << " hours, " << minutes << " minutes" << ";"
                   << fixed << setprecision(6) << timeToMaturityInYears << ";"
                   << fixed << setprecision(1) << risk_free_rate;

        if (precioCall > 0 && underBidValue > 0 && underAskValue > 0) { // check para valores positivos y calculo de la volatilidad con todos los datos
            double impliedVol = findImpliedVolatilityBoost(precioSubyacente, 1033.0, timeToMaturityInYears, risk_free_rate, precioCall);
            outputFile << ";" << fixed << setprecision(4) << impliedVol;
        } else {
            outputFile << ";N/A";
        }
        outputFile << endl;
    }
    file.close();
    outputFile.close();

    cout << "Output saveado a excel1.csv" << endl; //exporto a csv

    return 0;
}
