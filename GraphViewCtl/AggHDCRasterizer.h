#pragma once

#include "agg_pixfmt_rgba.h"

namespace agg
{

//========================================================================
   class hdc_rasterizer
   {
      enum
      {
         points_cluster_size   = 4094,
         polygons_cluster_size = 256
      };

   public:
      ~hdc_rasterizer();
      hdc_rasterizer(HDC dc);

     
//------------------------------------------------------------------------
      void line_color(agg::rgba8 c);
      void fill_color(agg::rgba8 c);
      void line_width(int width);
      void line_cap(agg::line_cap_e cap);
      void line_join(agg::line_join_e join);
      void fill_even_odd(bool even_odd_flag);

     
//------------------------------------------------------------------------
      void reset();
      void move_to(int x, int y);
      void line_to(int x, int y);
      void close_polygon();
      void render_fill_only();
      void render_stroke_only();
      void render_fill_and_stroke();
      void render_fill_with_line_color();

     
//------------------------------------------------------------------------
      template<class VertexSource>
      void add_path(VertexSource& vs, unsigned id=0)
      {
         double x;
         double y;

         unsigned flag;
         vs.rewind(id);
         while(!is_stop(flag = vs.vertex(&x, &y)))
         {
            if(is_vertex(flag))
            {
               if(is_move_to(flag)) 
               {
                  move_to(int(x), int(y));
               }
               else 
               {
                  line_to(int(x), int(y));
               }
            }
            else
            {
               if(is_close(flag)) close_polygon();
            }
         }
      }

   private:
      void create_pen();
      void realloc_points();
      void realloc_polygons();

      HDC       m_dc;
      unsigned  m_sat;
      HBRUSH    m_brush;
      HBRUSH    m_null_brush;
      HBRUSH    m_line_color_brush;
      HBRUSH    m_old_brush;
      HPEN      m_pen;
      HPEN      m_null_pen;
      HPEN      m_old_pen;
      int       m_line_width;
      int       m_line_cap;
      int       m_line_join;
      POINT*    m_points;
      int*      m_counts;
      bool*     m_closed;
      int       m_num_poly;
      int       m_num_points;
      int       m_max_poly;
      int       m_max_points;
      bool      m_reset_flag;
      LOGBRUSH  m_null_log_brush;
      LOGBRUSH  m_line_log_brush;
      rgba8     m_fill_color;
      rgba8     m_line_color;
   };

}; // namespace agg

