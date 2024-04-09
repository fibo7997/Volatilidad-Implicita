##importo csv de c++ con volatilidad implicita
import pandas as pd
import numpy as np

csv_volatilidad = r'C:\Users\Principal\Desktop\Neix\volatilidadImplicita.csv'

# Leo el csv
df = pd.read_csv(csv_volatilidad, sep=';', index_col=0, parse_dates=True)

##agarro el ultimo valor de cada dia (como un precio de cierre)
df2 = df.groupby(df['created_at'].dt.date).last().reset_index(drop=True)

# Calculo los retornos log diarios
df2['log_returns'] = np.log(df2['Precio Subyacente'] / df2['Precio Subyacente'].shift(1))

# Agrego columna con la volatilidad con valores diarios. Con una ventana de 5 dias (no rows) y con 252 trading days.
df2['volatilidadRealizada'] = df2.log_returns.rolling(window=5).std()*np.sqrt(252)

#defino date
df2['date'] = df2['created_at'].dt.date

##relleno valores de volatilidad realizada diaria, para cada dia correspondiente 
df2['volatilidadRealizada'] = df2.groupby('date')['volatilidadRealizada'].transform(lambda x: x.ffill().bfill())

##agrego volatilidad realizada a la dataframe original con la volatilidad implicita
df['volatilidadRealizada'] = df2['volatilidadRealizada']

####graficos
import matplotlib.pyplot as plt

plt.figure(figsize=(10, 6))

# Ploteo volatilidadRealizada
plt.plot(df['created_at'], df['volatilidadRealizada'], color='blue', label='volatilidadRealizada')

# Ploteo volatilidad implicita
plt.plot(df['created_at'], df['Implied_Vol'], color='red', label='Implied Volatility')

# Ploteo time to maturity
plt.plot(df['created_at'], df['Time to Maturity (Years)'], color='green', label='Time to Maturity')

plt.xlabel('Time')
plt.ylabel('Value')
plt.title('VolatilidadRealizada, Volatilidad Implicita, y Time to Maturity')
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.show()


###calculo spread y lo ploteo
df['spread'] = df['Implied_Vol'] - df['VolatilidadRealizada']

fig = plt.figure(figsize=(10,5))
plt.plot(df['spread'])
plt.grid()
plt.title('Spread')
plt.show();


# Identifico puntos donde el spread es mayor a 0.5 como posibles puntos de entrada y grafico
entry_points = (df['spread'] > 0.5)
# Plot the spread
plt.figure(figsize=(10, 6))
plt.plot(df['created_at'], df['spread'], color='blue', linestyle='-')
plt.scatter(df['created_at'][entry_points], df['spread'][entry_points], color='red', label='Entry Points')
plt.xlabel('Tiempo')
plt.ylabel('Spread')
plt.title('Spread Entre Volatilidad Implicita y realizada')
plt.grid(True)
plt.tight_layout()
plt.show()
df['entry_points'] = entry_points.map(lambda x: 'entry' if x else '')
#df.to_excel('outputfinal2.xlsx', index=False)
entry_count = (df['entry_points'] == 'entry').sum()

print("Number of entry points:", entry_count)
