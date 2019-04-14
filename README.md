# Autoscope - Latex template

## Dependencies

```
sudo apt-get install texlive-full python-pygments python-pip
sudo pip install pygments-style-monokailight
```

<br>

## Build

```
xelatex --shell-escape main.tex
```

<br>

## Provided commands

### Include code

Include a source code block :
```latex
\code{c}
void main(void){
    /*C code example*/
    }
\end{minted}
```

Include code in line :
```latex
\codeinline{text}{/*formatted text*/}
```

### Include figures

Set path to figures in `main.tex` :
```latex
\newcommand{\figures}{path/to/figures}
```

Include a figure :
```latex
\begin{figure}[H]
    \centering
    \includegraphics[width=0.3\linewidth]{\figures/image.png}
    \decoRule
    \caption[
    <Subtitle>]{
    <Subtitle>}
    \label{fig:<Subtitle>}
    \end{figure}
```

### Include equations

- `&` : Align columns
- `\\` : New ligne
- `^{}` : Superscript
- `_{}` : Subscript
- `\frac{}` : Fraction
- `\left(` & `\right)` : Scaled parenthesis
- `\left[` & `\right]` : Scaled braces

Example :
```latex
\begin{align*}
p(r,\theta,\varphi)&=G(\theta,\varphi)\cdot\left(\frac{P_{in}}{4\pi\cdot r^{2}}\right)&&(W/m^{2})\\
&=D(\theta,\varphi)\cdot\frac{P_{out}}{4\pi\cdot r^{2}}
\end{align*}
```

![example_equations](https://raw.githubusercontent.com/thibaudledo/Autoscope/latex/equations.png)

### Include lists

Use `[label=$\bullet$]` for first level :
```latex
\begin{itemize}[label=$\bullet$]
	\item <item1>
	\item <item2>
	\item \begin{itemize}
		\item <item3.1>
		\item <item3.2>
		\end{itemize}
	\end{itemize}
```
