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

### Source report complexity

With `-r`, Crawler now emits local and call-expanded complexity metrics in the Dabsic report.

Local metrics are computed while parsing each function: instructions, branches, loops, calls, cyclomatic approximation and maximum control nesting. Call-expanded metrics use the function graph collected during the same parse:

```dabsic
Report.Functions[0].Complexity.LocalScore = 12
Report.Functions[0].Complexity.CallExpandedScore = 31
Report.Functions[0].Complexity.PotentialExpandedScore = 44
Report.Functions[0].Complexity.CallDepth = 3
Report.Functions[0].Complexity.Recursive = false
Report.Functions[0].Complexity.CallsExternal = true
Report.Functions[0].Complexity.Partial = true
Report.Functions[0].Complexity.FunctionPointerEscape = false
```

`CallExpandedScore` follows direct calls to functions parsed in the same run. `PotentialExpandedScore` also accounts for function-pointer donation edges when the receiver is known. Scores are marked `Partial` when external calls, unresolved calls or recursive cycles prevent a complete closed-world estimate.


### CheckedReturn configuration

`CheckedReturn` verifies that critical return values are not silently ignored.
The default list covers allocation helpers and common POSIX/syscall-like functions
such as `malloc`, `realloc`, `open`, `read`, `write` and `close`.

The list can be extended or overridden from Dabsic:

```dabsic
CheckedReturn = true

# Keep the built-in list. Set to false to use only the configured list.
CheckedReturn.UseDefaultList = true

CheckedReturn.Functions[0].Name = "my_malloc"
CheckedReturn.Functions[0].Kind = "pointer"

CheckedReturn.Functions[1].Name = "my_realloc"
CheckedReturn.Functions[1].ReallocLike = true

CheckedReturn.Functions[2].Name = "my_write"
CheckedReturn.Functions[2].FullTransfer = true

# Disable one built-in function without removing the default list.
CheckedReturn.Functions[3].Name = "close"
CheckedReturn.Functions[3].Disabled = true
```

`FullTransfer` is intended for `write`-like calls whose return value must be
checked against the requested transfer size, not only against `-1`.
