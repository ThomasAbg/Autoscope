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
```
\code{c}
void main(void){
    /*C code example*/
    }
\end{minted}
```

Include code in line :
```
\codeinline{text}{/*formatted text*/}
```

### Include figures

Set path to figures in `main.tex` :
```
\newcommand{\figures}{path/to/figures}
```

Include a figure :
```
\begin{figure}[H]
    \centering
    \includegraphics[width=0.3\linewidth]{\figures/image.png}
    \decoRule
    \caption[
    Subtitle]{
    Subtitle}
    \label{fig:Subtitle}
    \end{figure}
```

