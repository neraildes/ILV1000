#ifndef SINGLETON_H
#define SINGLETON_H

#include "global.h"
#include "arduino.h"
#include <stddef.h>

#pragma once

class Singleton
{
   private:
       Singleton() = default; 
       //static Singleton* _instance; 
        
   public:
       static Singleton &getInstance();       
       Singleton(const Singleton &) = delete; // no copying
       Singleton &operator=(const Singleton &) = delete; 
         

                
};

extern Singleton &singleton;

#endif
