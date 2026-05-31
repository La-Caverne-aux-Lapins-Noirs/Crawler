# CCCCrawler

Configurable C Code Crawler.

Crawler est une bibliothèque de lecture/analyse de C et un binaire associé. Son usage principal reste la vérification de norme, mais le parseur sert aussi à produire des sorties d'analyse exploitables automatiquement.

## Vérification de norme

```sh
crawler -c [configuration]* [files]+ [--nocolor] [-v] [-I header_path]*
```

Les configurations supportées sont les fichiers Dabsic (`.dab`), JSON (`.json`), INI (`.ini`) et Lua (`.lua`). Si aucune configuration n'est fournie, la configuration par défaut installée est utilisée.

## Graphe d'appels

```sh
crawler -m -o output.dot [files]+ [-I header_path]*
```

Produit un fichier Graphviz/DOT représentant les appels entre fonctions du programme analysé.

Le graphe distingue notamment :

- les appels directs ;
- les appels récursifs ;
- les appels vers des fonctions extérieures/non définies dans les fichiers analysés ;
- les dons de pointeurs sur fonction, représentés par des flèches dédiées aux appels potentiels depuis l'extérieur.

## Rapport de métriques source

```sh
crawler -r [-o output.dab] [files]+ [-I header_path]*
```

Produit un rapport Dabsic. Sans `-o`, la sortie est écrite vers `/dev/stdout`.

Le rapport contient des totaux globaux et un détail par fonction. Il est destiné à être réutilisé par des scripts ou par d'autres outils Dabsic, par exemple pour organiser des concours de factorisation automatisés.

Structure indicative :

```dabsic
Report.Kind = "CrawlerSourceReport"
Report.Version = 1

Report.Totals.Functions = ...
Report.Totals.Instructions = ...
Report.Totals.Lines = ...
Report.Totals.Expressions = ...
Report.Totals.Declarations = ...
Report.Totals.Branches = ...
Report.Totals.Loops = ...
Report.Totals.Jumps = ...
Report.Totals.Returns = ...

Report.Functions[0].Name = "..."
Report.Functions[0].File = "..."
Report.Functions[0].StartLine = ...
Report.Functions[0].EndLine = ...
Report.Functions[0].LineCount = ...
Report.Functions[0].Instructions = ...
Report.Functions[0].Expressions = ...
Report.Functions[0].Declarations = ...
Report.Functions[0].Branches = ...
Report.Functions[0].Loops = ...
Report.Functions[0].Jumps = ...
Report.Functions[0].Returns = ...
```

`LineCount` ne compte que l'intérieur du corps de fonction, entre les accolades, sans la signature, sans la ligne de l'accolade ouvrante et sans la ligne de l'accolade fermante.

## Transpilation de prototypes

```sh
crawler -d [files]+
```

Cette sortie est réservée à la génération future d'un script Dabsic contenant prototypes et types.
