#include <iostream>
#include <fstream>
#include <cmath>
#include <chrono>
#include <thread>
#include <vector>
#include <mutex>


std::mutex mtx;

bool estPremier(int nombre)
{
    if (nombre <= 1) return false;
    if (nombre <= 3) return true;
    if (nombre % 2 == 0 || nombre % 3 == 0) return false;

    for (int i = 5; i <= std::sqrt(nombre); i += 6)
    {
        if (nombre % i == 0 || nombre % (i + 2) == 0)
            return false;
    }
    return true;
}

void genererNombresPremiers(int debut, int fin, const std::string& nomFichier)
{
    std::ofstream fichierSortie(nomFichier, std::ios_base::app);
    if (!fichierSortie)
    {
        std::cerr << "Erreur lors de l'ouverture du fichier " << nomFichier << std::endl;
        return;
    }

    for (int i = debut; i <= fin; ++i)
    {
        if (estPremier(i))
        {
            mtx.lock();
            fichierSortie << i << std::endl;
            mtx.unlock();
        }
    }
}

void nombresPremiers(int limite, const std::string& nomFichier)
{
    auto debut = std::chrono::high_resolution_clock::now();
    genererNombresPremiers(1, limite, nomFichier);
    auto fin = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duree = fin - debut;
    std::cout << "Temps pour trouver les nombres premiers jusqu'a " << limite << ": " << duree.count() << " secondes." << std::endl;
}

void nombresPremiersMultiCoeurs(int limite, const std::string& nomFichier)
{
    unsigned int numCores = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;

    auto debut = std::chrono::high_resolution_clock::now();


    for (unsigned int i = 0; i < numCores; ++i)
    {
        threads.emplace_back([=]()
            {
                int start = (limite / numCores) * i + 1;
                int end = (limite / numCores) * (i + 1);
                if (i == numCores - 1)
                    end = limite;

                genererNombresPremiers(start, end, nomFichier);
            });
    }


    for (auto& thread : threads)
    {
        thread.join();
    }

    auto fin = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duree = fin - debut;
    std::cout << "Temps pour trouver les nombres premiers jusqu'a " << limite << " avec " << numCores << " coeurs: " << duree.count() << " secondes." << std::endl;
}

int main()
{
    std::cout << "Nombre de coeurs (processeurs logiques) : " << std::thread::hardware_concurrency() << std::endl;


    nombresPremiers(10000, "nombresPremiers10000.txt");
    nombresPremiers(100000, "nombresPremiers100000.txt");
    nombresPremiers(1000000, "nombresPremiers1000000.txt");


    nombresPremiersMultiCoeurs(10000, "nombresPremiers10000Multi.txt");
    nombresPremiersMultiCoeurs(100000, "nombresPremiers100000Multi.txt");
    nombresPremiersMultiCoeurs(1000000, "nombresPremiers1000000Multi.txt");

    return 0;
}