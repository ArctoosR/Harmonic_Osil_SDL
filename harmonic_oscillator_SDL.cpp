/*
 Copyright 2020-2021 Abolfazl_Rodsarabi

 */

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>

#include <boost/numeric/odeint.hpp>
#include <boost/math/special_functions/round.hpp>
#include <SDL.h>

SDL_Window* window;
SDL_Renderer* render;
SDL_Surface* surface;
inline void plot(int x, int y, double brightness, Uint8 r, Uint8 g, Uint8 b, SDL_Renderer* render)
{
    SDL_SetRenderDrawColor(render, r, g, b, brightness * 255.0);
    SDL_RenderDrawPoint(render, x, y);
}



 //[ rhs_function
 /* The type of container used to hold the state vector */
typedef std::vector< double > state_type;

const double gam = 0.05;

/* The rhs of x' = f(x) */
void harmonic_oscillator(const state_type& x, state_type& dxdt, const double /* t */)
{
    dxdt[0] = x[1];
    dxdt[1] = -x[0] - gam * x[1];
}
//]





//[ rhs_class
/* The rhs of x' = f(x) defined as a class */
class harm_osc {

    double m_gam;

public:
    harm_osc(double gam) : m_gam(gam) { }

    void operator() (const state_type& x, state_type& dxdt, const double /* t */)
    {
        dxdt[0] = x[1];
        dxdt[1] = -x[0] - m_gam * x[1];
    }
};
//]





//[ integrate_observer
struct push_back_state_and_time
{
    std::vector< state_type >& m_states;
    std::vector< double >& m_times;

    push_back_state_and_time(std::vector< state_type >& states, std::vector< double >& times)
        : m_states(states), m_times(times) { }

    void operator()(const state_type& x, double t)
    {
        m_states.push_back(x);
        m_times.push_back(t);
    }
};
//]

struct write_state
{
    void operator()(const state_type& x) const
    {
        std::cout << x[0] << "\t" << x[1] << "\n";
    }
};


int main(int /* argc */, char** /* argv */)
{
    using namespace std;
    using namespace boost::numeric::odeint;



    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(640, 480, 0, &window, &render);
    SDL_SetRenderDrawColor(render, 255, 255, 255, 0);
    SDL_RenderClear(render);

  
    state_type x(2);
    x[0] = 1.0; // start at x=1.0, p=0.0
    x[1] = 0.0;

    vector<state_type> x_vec;
    vector<double> times;

   size_t  steps = integrate(harmonic_oscillator,
        x, 0.0, 300.0, 0.1,
        push_back_state_and_time(x_vec, times));

    /* output */
    for (size_t i = 0; i <= steps; i++)
    {
        cout << times[i] << '\t' << boost::math::round((1 + x_vec[i][0]) * 100)  << '\t' << boost::math::round(( 1 + x_vec[i][1]) * 100 ) << '\n';
        plot(boost::math::round(times[i]), boost::math::round((1 + x_vec[i][0]) * 100), 255, 0, 0, 255, render);
        plot(boost::math::round(times[i]), boost::math::round((1 + x_vec[i][1]) * 100) + 200, 255, 255, 0, 0, render);
    }
    


    SDL_RenderPresent(render);


    while (1);


}