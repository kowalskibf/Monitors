#include <pthread.h>
#include <iostream>
#include <fstream>
#include <random>
#include <unistd.h>

std::string storageFileName = "storage.txt";

bool debug = true;
bool paramsSuccess = false;

int storage, products;

int k, n, m, a, b, c, d;

int getRandomInt(int a, int b)
{
    return rand() % (b - a + 1) + a;
}

bool isInteger(std::string s)
{
    try
    {
        std::stoi(s);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

class Monitor
{
private:
    pthread_cond_t consumerCond, producerCond;
    pthread_mutex_t mutex;
    bool isProducerTurn;

public:
    Monitor()
    {
        pthread_mutex_init(&mutex, NULL);
        pthread_cond_init(&producerCond, NULL);
        pthread_cond_init(&consumerCond, NULL);
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&producerCond);
        isProducerTurn = true;
    }

    void producerEnter(long thread_id)
    {
        if (debug)
            std::cout << "Producer " << thread_id << " waiting...\n";
        pthread_mutex_lock(&mutex);
        while (!isProducerTurn)
            pthread_cond_wait(&producerCond, &mutex);

        if (debug)
            std::cout << "Producer " << thread_id << " entered the critical section\n";
        std::ifstream storageFileIn(storageFileName);
        storageFileIn >> storage;
        storageFileIn.close();
        products = getRandomInt(c, d);
        std::ofstream storageFileOut(storageFileName);
        std::fstream logFile("log.txt", std::ios::app);
        std::string fileName = "producer" + std::to_string(thread_id) + "log.txt";
        std::fstream localLogFile(fileName, std::ios::app);
        if (storage + products <= k)
        {
            storage += products;
            logFile << "Producer " << thread_id << " delivered " << products << " piece(s), storage: " << storage << " piece(s)\n";
            localLogFile << "I delivered " << products << " piece(s), storage: " << storage << " piece(s)\n";
            if (debug)
                std::cout << "Producer " << thread_id << " delivered " << products << " piece(s), storage: " << storage << " piece(s)\n";
            storageFileOut << storage;
        }
        else
        {
            logFile << "Producer " << thread_id << " failed to deliver " << products << " piece(s), storage: " << storage << " piece(s)\n";
            localLogFile << "I failed to deliver " << products << " piece(s), storage: " << storage << " piece(s)\n";
            if (debug)
                std::cout << "Producer " << thread_id << " failed to deliver " << products << " piece(s), storage: " << storage << " piece(s)\n";
        }
        storageFileOut.close();
        logFile.close();
        localLogFile.close();
        sleep(1);
        if (debug)
            std::cout << "Producer " << thread_id << " left the critical section\n";

        isProducerTurn = false;
        pthread_cond_signal(&consumerCond);
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }

    void consumerEnter(long thread_id)
    {
        if (debug)
            std::cout << "Consumer " << thread_id << " waiting...\n";
        pthread_mutex_lock(&mutex);
        while (isProducerTurn)
            pthread_cond_wait(&consumerCond, &mutex);

        if (debug)
            std::cout << "Consumer " << thread_id << " entered the critical section\n";
        std::ifstream storageFileIn(storageFileName);
        storageFileIn >> storage;
        storageFileIn.close();
        products = getRandomInt(a, b);
        std::ofstream storageFileOut(storageFileName);
        std::fstream logFile("log.txt", std::ios::app);
        std::string fileName = "consumer" + std::to_string(thread_id) + "log.txt";
        std::fstream localLogFile(fileName, std::ios::app);
        if (products <= storage)
        {
            storage -= products;
            logFile << "Consumer " << thread_id << " picked " << products << " piece(s), storage: " << storage << " piece(s)\n";
            localLogFile << "I picked " << products << " piece(s), storage: " << storage << " piece(s)\n";
            if (debug)
                std::cout << "Consumer " << thread_id << " picked " << products << " piece(s), storage: " << storage << " piece(s)\n";
            storageFileOut << storage;
        }
        else
        {
            logFile << "Consumer " << thread_id << " failed to pick " << products << " piece(s), storage: " << storage << " piece(s)\n";
            localLogFile << "I failed to pick " << products << " piece(s), storage: " << storage << " piece(s)\n";
            if (debug)
                std::cout << "Consumer " << thread_id << " failed to pick " << products << " piece(s), storage: " << storage << " piece(s)\n";
        }
        storageFileOut.close();
        logFile.close();
        localLogFile.close();
        sleep(1);
        if (debug)
            std::cout << "Consumer " << thread_id << " left the critical section\n";

        isProducerTurn = true;
        pthread_cond_signal(&producerCond);
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
};

Monitor monitor;

void *producer(void *arg)
{
    long thread_id = (long)arg;

    while (1)
    {
        monitor.producerEnter(thread_id);
    }

    return NULL;
}

void *consumer(void *arg)
{
    long thread_id = (long)arg;

    while (1)
    {
        monitor.consumerEnter(thread_id);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc >= 2)
    {
        try
        {
            k = std::stoi(argv[1]);
            n = std::stoi(argv[2]);
            m = std::stoi(argv[3]);
            a = std::stoi(argv[4]);
            b = std::stoi(argv[5]);
            c = std::stoi(argv[6]);
            d = std::stoi(argv[7]);
            paramsSuccess = true;
        }
        catch (...)
        {
            paramsSuccess = false;
            std::cerr << "Warning: Failed to fetch paremeters from the command line. Executing with parameters provided in the params file\n";
        }
    }

    if (!paramsSuccess)
    {
        try
        {
            std::ifstream paramsFile("params.soi_homework_3_custom_extension");
            paramsFile >> k >> n >> m >> a >> b >> c >> d;
            paramsFile.close();
            paramsSuccess = true;
        }
        catch (...)
        {
            paramsSuccess = false;
            std::cerr << "Warning: Failed to fetch parameters from the params file. Fetching default parameters\n";
        }
    }

    if (!paramsSuccess)
    {
        k = 50;
        n = 4;
        m = 7;
        a = 1;
        b = 15;
        c = 1;
        d = 15;
    }

    std::string userWantsToInputManually;
    std::string input;
    std::cout << "Would you like to input parameters' values manually? Leave empty if not: ";
    std::getline(std::cin, userWantsToInputManually);
    if (!userWantsToInputManually.empty())
    {
        std::cout << "Provide a new value for storage size k (leave empty to use default " << k << "): ";
        std::getline(std::cin, input);
        while (!isInteger(input) || std::stoi(input) < 0 || std::stoi(input) > 999999999)
        {
            if (!isInteger(input))
                break;
            std::cout << "Must be a number within a range of [0, 999999999]" << std::endl;
            std::getline(std::cin, input);
        }
        if (isInteger(input))
            k = std::stoi(input);
        std::cout << "Provide a new value for consumers' count n (leave empty to use default " << n << "): ";
        std::getline(std::cin, input);
        while (!isInteger(input) || std::stoi(input) < 1 || std::stoi(input) > 999999999)
        {
            if (!isInteger(input))
                break;
            std::cout << "Must be a number within a range of [1, 999999999]" << std::endl;
            std::getline(std::cin, input);
        }
        if (isInteger(input))
            n = std::stoi(input);
        std::cout << "Provide a new value for producers' count m (leave empty to use default " << m << "): ";
        std::getline(std::cin, input);
        while (!isInteger(input) || std::stoi(input) < 1 || std::stoi(input) > 999999999)
        {
            if (!isInteger(input))
                break;
            std::cout << "Must be a number within a range of [1, 999999999]" << std::endl;
            std::getline(std::cin, input);
        }
        if (isInteger(input))
            m = std::stoi(input);
        std::cout << "Provide a new value for the lower bound of consumer's demand a (leave empty to use default " << a << "): ";
        std::getline(std::cin, input);
        while (!isInteger(input) || std::stoi(input) < 0 || std::stoi(input) > 999999999)
        {
            if (!isInteger(input))
                break;
            std::cout << "Must be a number within a range of [0, 999999999]" << std::endl;
            std::getline(std::cin, input);
        }
        if (isInteger(input))
            a = std::stoi(input);
        std::cout << "Provide a new value for the upper bound of consumer's demand b (leave empty to use default " << b << "): ";
        std::getline(std::cin, input);
        while (!isInteger(input) || std::stoi(input) < 0 || std::stoi(input) > 999999999 || std::stoi(input) < a)
        {
            if (!isInteger(input))
                break;
            std::cout << "Must be a number within a range of [0, 999999999] and greater than last number" << std::endl;
            std::getline(std::cin, input);
        }
        if (isInteger(input))
            b = std::stoi(input);
        std::cout << "Provide a new value for the lower bound of producer's supply c (leave empty to use default " << c << "): ";
        std::getline(std::cin, input);
        while (!isInteger(input) || std::stoi(input) < 0 || std::stoi(input) > 999999999)
        {
            if (!isInteger(input))
                break;
            std::cout << "Must be a number within a range of [0, 999999999]" << std::endl;
            std::getline(std::cin, input);
        }
        if (isInteger(input))
            c = std::stoi(input);
        std::cout << "Provide a new value for the upper bound of producer's supply d (leave empty to use default " << d << "): ";
        std::getline(std::cin, input);
        while (!isInteger(input) || std::stoi(input) < 0 || std::stoi(input) > 999999999 || std::stoi(input) < c)
        {
            if (!isInteger(input))
                break;
            std::cout << "Must be a number within a range of [0, 999999999] and greater than last number" << std::endl;
            std::getline(std::cin, input);
        }
        if (isInteger(input))
            d = std::stoi(input);
    }

    if (k < 0)
    {
        std::cerr << "Error: Storage size cannot be negative\n";
        return 1;
    }
    if (k > 999999999)
    {
        std::cerr << "Error: Storage size must be lower than 1 billion\n";
        return 1;
    }
    if (n < 1)
    {
        std::cerr << "Error: There must be at least one consumer\n";
        return 1;
    }
    if (n > 999999999)
    {
        std::cerr << "Error: Consumers' count must be lower than 1 billion\n";
        return 1;
    }
    if (m < 1)
    {
        std::cerr << "Error: There must be at least one producer\n";
        return 1;
    }
    if (m > 999999999)
    {
        std::cerr << "Error: Producers' count must be lower than 1 billion\n";
        return 1;
    }
    if (a < 0)
    {
        std::cerr << "Error: Lower bound of consumer's demand cannot be negative\n";
        return 1;
    }
    if (a > 999999999)
    {
        std::cerr << "Error: Lower bound of consumer's demand must be lower than 1 billion\n";
        return 1;
    }
    if (b < 0)
    {
        std::cerr << "Error: Upper bound of consumer's demand cannot be negative\n";
        return 1;
    }
    if (b > 999999999)
    {
        std::cerr << "Error: Upper bound of consumer's demand must be lower than 1 billion\n";
        return 1;
    }
    if (a > b)
    {
        std::cerr << "Error: Lower bound of consumer's demand cannot be higher than upper bound\n";
        return 1;
    }
    if (c < 0)
    {
        std::cerr << "Error: Lower bound of producer's supply cannot be negative\n";
        return 1;
    }
    if (c > 999999999)
    {
        std::cerr << "Error: Lower bound of producer's supply must be lower than 1 billion\n";
        return 1;
    }
    if (d < 0)
    {
        std::cerr << "Error: Upper bound of producer's supply cannot be negative\n";
        return 1;
    }
    if (d > 999999999)
    {
        std::cerr << "Error: Upper bound of producer's supply must be lower than 1 billion\n";
        return 1;
    }
    if (c > d)
    {
        std::cerr << "Error: Lower bound of producer's supply cannot be higher than upper bound\n";
        return 1;
    }

    std::ofstream storageFile(storageFileName);
    storageFile << std::round(k / 2);
    storageFile.close();
    std::ofstream logFile("log.txt", std::ios::trunc);
    logFile.close();
    std::ofstream logFile2("log.txt");
    logFile2 << "Storage at the beginning: " << std::round(k / 2) << " piece(s)\n";
    logFile2.close();

    for (int i = 1; i <= m; i++)
    {
        std::string fileName = "producer" + std::to_string(i) + "log.txt";
        std::ofstream localLogFile(fileName, std::ios::trunc);
        localLogFile.close();
    }
    for (int i = 1; i <= n; i++)
    {
        std::string fileName = "consumer" + std::to_string(i) + "log.txt";
        std::ofstream localLogFile(fileName, std::ios::trunc);
        localLogFile.close();
    }

    pthread_t producerThreads[m];
    pthread_t consumerThreads[n];
    for (long i = 1; i <= m; ++i)
        pthread_create(&producerThreads[i - 1], NULL, producer, (void *)i);
    for (long i = 1; i <= n; ++i)
        pthread_create(&consumerThreads[i - 1], NULL, consumer, (void *)i);
    for (int i = 0; i < m; ++i)
        pthread_join(producerThreads[i], NULL);
    for (int i = 0; i < n; ++i)
        pthread_join(consumerThreads[i], NULL);
}