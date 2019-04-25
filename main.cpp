#include <stdio.h>
#include "readosm.h"
#include "SDL2/SDL.h"
#include <vector>
#include <iostream>
#include <map>
#include "SDL2/SDL2_gfxPrimitives.h"
#include "earcut.hpp"

struct NodeData
{
    double lat;
    double lon;
};

struct TagData
{
    std::string key;
    std::string val;
};

struct WayData
{
    int node_ref_count;
    std::vector<long long> nodeIDs;
    std::vector<TagData> tags;
};

struct osm_statistics
{
    double min_longitude;
    double max_longitude;
    double min_latitude;
    double max_latitude;
};

std::map<long long, NodeData> Nodes;
std::vector<WayData> Ways;

static int print_node (const void *user_data, const readosm_node * node)
{
    char buf[128];
    int i;
    const readosm_tag *tag;
    if (user_data != NULL)
        user_data = NULL;       /* silencing stupid compiler warnings */
#if defined(_WIN32) || defined(__MINGW32__)
    /* CAVEAT - M$ runtime doesn't supports %lld for 64 bits */
    sprintf (buf, "%I64d", node->id);
#else
    sprintf (buf, "%lld", node->id);
#endif
    printf ("\t<node id=\"%s\"", buf);
    /*
    * some individual values may be set, or may be not
    * unset values are identified by the READOSM_UNDEFINED
    * conventional value, and must be consequently ignored
    */
    if (node->latitude != READOSM_UNDEFINED)
        printf (" lat=\"%1.7f\"", node->latitude);
    if (node->longitude != READOSM_UNDEFINED)
        printf (" lon=\"%1.7f\"", node->longitude);
    if (node->version != READOSM_UNDEFINED)
        printf (" version=\"%d\"", node->version);
    if (node->changeset != READOSM_UNDEFINED)
      {
#if defined(_WIN32) || defined(__MINGW32__)
          /* CAVEAT - M$ runtime doesn't supports %lld for 64 bits */
          sprintf (buf, "%I64d", node->changeset);
#else
          sprintf (buf, "%lld", node->changeset);
#endif
          printf (" changeset=\"%s\"", buf);
      }
    /*
    * unset string values are identified by a NULL pointer
    * and must be consequently ignored
    */
    if (node->user != NULL)
        printf (" user=\"%s\"", node->user);
    if (node->uid != READOSM_UNDEFINED)
        printf (" uid=\"%d\"", node->uid);
    if (node->timestamp != NULL)
        printf (" timestamp=\"%s\"", node->timestamp);
    /*
    * the Node object may have its own tag list
    * please note: this one is a variable-length list,
    * and may be empty: in this case tag_count will be ZERO
    */
    if (node->tag_count == 0)
        printf (" />\n");
    else
      {
          printf (">\n");
          for (i = 0; i < node->tag_count; i++)
            {
                /* we'll now print each <tag> for this node */
                tag = node->tags + i;
                printf ("\t\t<tag k=\"%s\" v=\"%s\" />\n", tag->key,
                        tag->value);
            }
          printf ("\t</node>\n");
      }
    return READOSM_OK;
}

static int draw_node (const void *user_data, const readosm_node * node)
{

    struct osm_statistics *stats = (struct osm_statistics *) user_data;

    if (node->latitude != READOSM_UNDEFINED)
    {
    if (node->latitude > stats->max_latitude)
        stats->max_latitude = node->latitude;
    if (node->latitude < stats->min_latitude)
        stats->min_latitude = node->latitude;
    }
    if (node->longitude != READOSM_UNDEFINED)
    {
        if (node->longitude > stats->max_longitude)
            stats->max_longitude = node->longitude;
        if (node->longitude < stats->min_longitude)
            stats->min_longitude = node->longitude;
    }

    NodeData nodeData;
    nodeData.lat = node->latitude;
    nodeData.lon = node->longitude;
    Nodes[node->id] = nodeData;

    return READOSM_OK;
}

static int print_way (const void *user_data, const readosm_way * way)
{
    /*
    char buf[128];
    int i;
    const readosm_tag *tag;

    WayData wayData;
    wayData.node_ref_count = way->node_ref_count;

    sprintf (buf, "%lld", way->id);

    printf ("\t<way id=\"%s\"", buf);

    if (way->version != READOSM_UNDEFINED)
        printf (" version=\"%d\"", way->version);
    if (way->changeset != READOSM_UNDEFINED)
      {
#if defined(_WIN32) || defined(__MINGW32__)

          sprintf (buf, "%I64d", way->changeset);
#else
          sprintf (buf, "%lld", way->changeset);
#endif
          printf (" changeset=\"%s\"", buf);
      }

    if (way->user != NULL)
        printf (" user=\"%s\"", way->user);
    if (way->uid != READOSM_UNDEFINED)
        printf (" uid=\"%d\"", way->uid);
    if (way->timestamp != NULL)
        printf (" timestamp=\"%s\"", way->timestamp);

    if (way->tag_count == 0 && way->node_ref_count == 0)
        printf (" />\n");
    else
      {
          printf (">\n");
          for (i = 0; i < way->node_ref_count; i++)
            {

#if defined(_WIN32) || defined(__MINGW32__)

                sprintf (buf, "%I64d", *(way->node_refs + i));
#else
                sprintf (buf, "%lld", *(way->node_refs + i));
#endif
                printf ("\t\t<nd ref=\"%s\" />\n", buf);
                wayData.nodeIDs.push_back(way->node_refs[i]);
            }
          for (i = 0; i < way->tag_count; i++)
            {

                tag = way->tags + i;
                printf ("\t\t<tag k=\"%s\" v=\"%s\" />\n", tag->key, tag->value);

                newTag.key = tag->key;
                newTag.val = tag->value;
                wayData.tags.push_back(newTag);
                std::cout << wayData.tags.size() << std::endl;
            }
          printf ("\t</way>\n");
      }


    ways.push_back(wayData);
    return READOSM_OK;
*/
}

static int draw_way (const void *user_data, const readosm_way * way)
{
    char buf[128];
    int i;
    const readosm_tag *tag;

    Ways.push_back(WayData());

    sprintf (buf, "%lld", way->id);

    printf ("id: %s\n", buf);


    if (way->user != NULL)
        printf ("user: %s\n", way->user);
    if (way->uid != READOSM_UNDEFINED)
        printf ("uid: %d\n", way->uid);
    if (way->timestamp != NULL)
        printf ("timestamp: %s\n", way->timestamp);
    /*
    * the Way object may have a noderefs-list and a tag-list
    * please note: these are variable-length lists, and may
    * be empty: in this case the corresponding item count
    * will be ZERO
    */
    if (way->tag_count == 0 && way->node_ref_count == 0){}
    else
    {
        for (i = 0; i < way->node_ref_count; i++)
        {
            sprintf (buf, "%lld", *(way->node_refs + i));
            printf ("nodeRef: %s\n", buf);
            Ways.back().nodeIDs.push_back(*(way->node_refs + i));
        }
        for (i = 0; i < way->tag_count; i++)
        {
            /* we'll now print each <tag> for this way */
            tag = way->tags + i;
            printf ("tag: key: %s val:%s\n", tag->key, tag->value);
            TagData tagDat;
            tagDat.key = tag->key;
            tagDat.val = tag->value;
            Ways.back().tags.push_back(tagDat);
        }
    }
    return READOSM_OK;
}

static int print_relation (const void *user_data, const readosm_relation * relation)
{
    char buf[128];
    int i;
    const readosm_member *member;
    const readosm_tag *tag;
    if (user_data != NULL)
        user_data = NULL;       /* silencing stupid compiler warnings */
#if defined(_WIN32) || defined(__MINGW32__)
    /* CAVEAT - M$ runtime doesn't supports %lld for 64 bits */
    sprintf (buf, "%I64d", relation->id);
#else
    sprintf (buf, "%lld", relation->id);
#endif
    printf ("\t<relation id=\"%s\"", buf);
    /*
    * some individual values may be set, or may be not
    * unset values are identified by the READOSM_UNDEFINED
    * conventional value, and must be consequently ignored
    */
    if (relation->version != READOSM_UNDEFINED)
        printf (" version=\"%d\"", relation->version);
    if (relation->changeset != READOSM_UNDEFINED)
      {
#if defined(_WIN32) || defined(__MINGW32__)
          /* CAVEAT - M$ runtime doesn't supports %lld for 64 bits */
          sprintf (buf, "%I64d", relation->changeset);
#else
          sprintf (buf, "%lld", relation->changeset);
#endif
          printf (" changeset=\"%s\"", buf);
      }
    /*
    * unset string values are identified by a NULL pointer
    * and must be consequently ignored
    */
    if (relation->user != NULL)
        printf (" user=\"%s\"", relation->user);
    if (relation->uid != READOSM_UNDEFINED)
        printf (" uid=\"%d\"", relation->uid);
    if (relation->timestamp != NULL)
        printf (" timestamp=\"%s\"", relation->timestamp);
    /*
    * the Relation object may have a member-list and a tag-list
    * please note: these are variable-length lists, and may
    * be empty: in this case the corresponding item count
    * will be ZERO
    */
    if (relation->tag_count == 0 && relation->member_count == 0)
        printf (" />\n");
    else
      {
          printf (">\n");
          for (i = 0; i < relation->member_count; i++)
            {
                /* we'll now print each <member> for this way */
                member = relation->members + i;
#if defined(_WIN32) || defined(__MINGW32__)
                /* CAVEAT - M$ runtime doesn't supports %lld for 64 bits */
                sprintf (buf, "%I64d", member->id);
#else
                sprintf (buf, "%lld", member->id);
#endif
                /* any <member> may be of "node", "way" or "relation" type */
                switch (member->member_type)
                  {
                  case READOSM_MEMBER_NODE:
                      printf ("\t\t<member type=\"node\" ref=\"%s\"", buf);
                      break;
                  case READOSM_MEMBER_WAY:
                      printf ("\t\t<member type=\"way\" ref=\"%s\"", buf);
                      break;
                  case READOSM_MEMBER_RELATION:
                      printf ("\t\t<member type=\"relation\" ref=\"%s\"", buf);
                      break;
                  default:
                      printf ("\t\t<member ref=\"%s\"", buf);
                      break;
                  };
                if (member->role != NULL)
                    printf (" role=\"%s\" />\n", member->role);
                else
                    printf (" />\n");
            }
          for (i = 0; i < relation->tag_count; i++)
            {
                /* we'll now print each <tag> for this way */
                tag = relation->tags + i;
                printf ("\t\t<tag k=\"%s\" v=\"%s\" />\n", tag->key,
                        tag->value);
            }
          printf ("\t</relation>\n");
      }
    return READOSM_OK;
}

static int draw_relation (const void *user_data, const readosm_relation * relation)
{
    return READOSM_OK;
}

struct osm_statistics infos;

int loadOSM()
{
    const void *osm_handle;
    int ret;
    infos.min_longitude = 180.0;
    infos.max_longitude = -180.0;
    infos.min_latitude = 90.0;
    infos.max_latitude = -90.0;
    ret = readosm_open ("/home/tim/Downloads/map(1).osm", &osm_handle);
    if (ret != READOSM_OK)
    {
      fprintf (stderr, "OPEN error: %d\n", ret);
      goto stop;
    }

    ret = readosm_parse (osm_handle, &infos, draw_node, draw_way, draw_relation);
    if (ret != READOSM_OK)
    {
      fprintf (stderr, "PARSE error: %d\n", ret);
      goto stop;
    }
    fprintf (stderr, "Ok, OSM input file successfully parsed\n");
  stop:

    readosm_close (osm_handle);
    return 0;
}

double map(double x, double in_min, double in_max, double out_min, double out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void setColorByTag(SDL_Color &color, bool &area, std::string key, std::string value)
{
    if (key == "highway")
    {
        if (value == "footway")
        {
            color.r = 255;
            color.g = 255;
            color.b = 0;
            color.a = 255;
        }
        if (value == "tertiary")
        {
            color.r = 255;
            color.g = 255;
            color.b = 255;
            color.a = 255;
        }
    }
    if (value == "grass")
    {
        color.r = 0;
        color.g = 255;
        color.b = 0;
        color.a = 255;
        area = true;
    }
    if (value == "forest")
    {
        color.r = 63;
        color.g = 135;
        color.b = 65;
        color.a = 255;
        area = true;
    }
}

void renderOSM(SDL_Window *window, SDL_Renderer *renderer)
{
    //filledTrigonRGBA(renderer, 0, 0, 100, 100, 0, 100, 255, 255, 255, 255);
    for (int i = 0; i < Ways.size(); i++)
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
        for (int y = 0; y < Ways[i].tags.size(); y++)
        {
            //setColorByTag(renderer, Ways[i].tags[y].key, Ways[i].tags[y].val);
        }

        for (int y = 1; y < Ways[i].nodeIDs.size(); y++)
        {
            SDL_RenderDrawLine(renderer,
                               map(Nodes[ Ways[i].nodeIDs[y] ].lon, infos.min_longitude, infos.max_longitude, 0, 1280),
                               map(Nodes[ Ways[i].nodeIDs[y] ].lat, infos.min_latitude, infos.max_latitude, 800, 0),

                               map(Nodes[ Ways[i].nodeIDs[y-1] ].lon, infos.min_longitude, infos.max_longitude, 0, 1280),
                               map(Nodes[ Ways[i].nodeIDs[y-1] ].lat, infos.min_latitude, infos.max_latitude, 800, 0) );
        }
    }

    for (auto x : Nodes)
    {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawPoint(renderer,
                            map(x.second.lon, infos.min_longitude, infos.max_longitude, 0, 1280),
                            map(x.second.lat, infos.min_latitude, infos.max_latitude, 800, 0) );
    }
}

void renderOSM2(SDL_Window *window, SDL_Renderer *renderer)
{
    for (int i = 0; i < Ways.size(); i++)
    {
        SDL_Color polyColor;
        bool area = false;
        for (int y = 0; y < Ways[i].tags.size(); y++)
        {
            setColorByTag(polyColor, area, Ways[i].tags[y].key, Ways[i].tags[y].val);
        }

        if (area)
        {
            using Point = std::array<double, 2>;
            std::vector<std::vector<Point>> polygon;
            polygon.push_back(std::vector<Point>());
            for (int y = 0; y < Ways[i].nodeIDs.size(); y++)
            {
                Point pt;
                pt[0] = map(Nodes[ Ways[i].nodeIDs[y] ].lon, infos.min_longitude, infos.max_longitude, 0, 800);
                pt[1] = map(Nodes[ Ways[i].nodeIDs[y] ].lat, infos.min_latitude, infos.max_latitude, 800, 0);
                polygon.back().push_back(pt);
            }
            std::vector<int> indices = mapbox::earcut<int>(polygon);
            for (int y = 0; y < indices.size(); y+=3)
            {
                filledTrigonRGBA(renderer, polygon.back()[indices[y]][0], polygon.back()[indices[y]][1] , polygon.back()[indices[y+1]][0], polygon.back()[indices[y+1]][1], polygon.back()[indices[y+2]][0], polygon.back()[indices[y+2]][1], polyColor.r, polyColor.g, polyColor.b, polyColor.a);
            }
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, polyColor.r, polyColor.g, polyColor.b, polyColor.a);

            for (int y = 1; y < Ways[i].nodeIDs.size(); y++)
            {
                SDL_RenderDrawLine(renderer,
                                   map(Nodes[ Ways[i].nodeIDs[y] ].lon, infos.min_longitude, infos.max_longitude, 0, 800),
                                   map(Nodes[ Ways[i].nodeIDs[y] ].lat, infos.min_latitude, infos.max_latitude, 800, 0),

                                   map(Nodes[ Ways[i].nodeIDs[y-1] ].lon, infos.min_longitude, infos.max_longitude, 0, 800),
                                   map(Nodes[ Ways[i].nodeIDs[y-1] ].lat, infos.min_latitude, infos.max_latitude, 800, 0) );
            }
        }
    }

    for (auto x : Nodes)
    {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawPoint(renderer,
                            map(x.second.lon, infos.min_longitude, infos.max_longitude, 0, 800),
                            map(x.second.lat, infos.min_latitude, infos.max_latitude, 800, 0) );
    }
}

int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        SDL_Window* window = NULL;
        SDL_Renderer* renderer = NULL;

        if (SDL_CreateWindowAndRenderer(800, 800, 0, &window, &renderer) == 0) {
            SDL_bool done = SDL_FALSE;
            loadOSM();
            std::cout << "minLat: " << infos.min_latitude << " minLon: " << infos.min_longitude << " maxLat: " << infos.max_latitude << " maxLon: " << infos.max_longitude << std::endl;
            while (!done) {
                SDL_Event event;

                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                SDL_RenderClear(renderer);

                renderOSM2(window, renderer);

                SDL_RenderPresent(renderer);

                while (SDL_PollEvent(&event)) {
                    if (event.type == SDL_QUIT) {
                        done = SDL_TRUE;
                    }
                }
            }
        }

        if (renderer) {
            SDL_DestroyRenderer(renderer);
        }
        if (window) {
            SDL_DestroyWindow(window);
        }
    }
    SDL_Quit();
    return 0;
}
