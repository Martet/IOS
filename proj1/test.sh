#!/bin/bash

POSIXLY_CORRECT=yes

#Vypíše zprávné použití, ukončí skript
usage () {
    echo "Použití: tradelog [-h|--help] [FILTR] [PŘÍKAZ] [LOG [LOG2 [...]]"

    if [ "$1" = "full" ]; then
        echo
        echo "PŘÍKAZ může být jeden z:"
        echo "list-tick – výpis seznamu vyskytujících se burzovních symbolů, tzv. “tickerů”."
        echo "profit – výpis celkového zisku z uzavřených pozic."
        echo "pos – výpis hodnot aktuálně držených pozic seřazených sestupně dle hodnoty."
        echo "last-price – výpis poslední známé ceny pro každý ticker."
        echo "hist-ord – výpis histogramu počtu transakcí dle tickeru."
        echo "graph-pos – výpis grafu hodnot držených pozic dle tickeru."
        echo
        echo "FILTR může být kombinace následujících:"
        echo "-a DATETIME – after: jsou uvažovány pouze záznamy PO tomto datu (bez tohoto data). DATETIME je formátu YYYY-MM-DD HH:MM:SS."
        echo "-b DATETIME – before: jsou uvažovány pouze záznamy PŘED tímto datem (bez tohoto data)."
        echo "-t TICKER – jsou uvažovány pouze záznamy odpovídající danému tickeru. Při více výskytech přepínače se bere množina všech uvedených tickerů."
        echo "-w WIDTH – u výpisu grafů nastavuje jejich šířku, tedy délku nejdelšího řádku na WIDTH. Tedy, WIDTH musí být kladné celé číslo. Více výskytů přepínače je chybné spuštění."
        echo "-h a --help vypíšou nápovědu s krátkým popisem každého příkazu a přepínače."
        exit 0
    fi

    exit 1
}

#parsování flagů
i=1
while [ $i -lt $# ]; do
    case "${!i}" in
        -a)  ((i++)); after_date="${!i}";;
        -b)  ((i++)); before_date="${!i}";;
        -t)  ((i++)); tickers+="${!i} ";;
        -w)  ((i++)); width="${!i}";;
        -h | --help) usage "full";;
        list-tick) cmd="list";;
        profit) cmd="profit";;
        pos) cmd="pos";;
        last-price) cmd="last";;
        hist-ord) cmd="hist";;
        graph-pos) cmd="graph";;
        ?)  files+="${!i} "
    esac
    ((i++))
done