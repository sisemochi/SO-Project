#!/bin/bash

# Verifică dacă scriptul a primit exact un argument
if [ "$#" -ne 1 ]; then
    echo "Utilizare: bash $0 <caracter>"
    exit 1
fi

caracter=$1
contor=0

# Funcție pentru verificarea dacă o propoziție este corectă
este_propozitie_corecta() {
    local propozitie=$1
    [[ $propozitie =~ ^[A-Z] && $propozitie =~ [$caracter] && $propozitie =~ [\.!?]$ && ! $propozitie =~ ,\ și ]]
}

# Citirea liniilor până la EOF
while IFS= read -r linie; do
    if este_propozitie_corecta "$linie"; then
        ((contor++))
    fi
done

echo ""
echo "$contor"