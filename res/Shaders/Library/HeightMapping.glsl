#define RELIEF_LINEAR_STEPS 30
#define RELIEF_BINARY_STEPS 8
#define NORMAL_SAMPLE_EPSILON 0.0005


vec3 ParallaxOcclusionMapping_Offset(sampler2D heightmap, vec2 uv, vec3 viewDirTS, float scale)
{
    /// Returns tangent space offset

    vec3 rayPos = vec3(uv, 0);
    float h;

    // Linear search
    float stepSize = 1.0 / float(RELIEF_LINEAR_STEPS);
    for(int i = 1; i < RELIEF_LINEAR_STEPS; i++)
    {
        h = texture(heightmap, rayPos.xy).r;

        if (rayPos.z / scale + 1.0 < h)
            break;

        rayPos += viewDirTS * stepSize;
    }

    // Binary search
    vec3 bestHit = rayPos;
    for(int i = 0; i < RELIEF_BINARY_STEPS; i++)
    {
        stepSize *= 0.5;
        h = texture(heightmap, rayPos.xy).r;

        if(rayPos.z / scale + 1.0 < h)
        {
            bestHit = rayPos;
            rayPos += -viewDirTS * stepSize;
        }
        else
            rayPos += viewDirTS * stepSize;
    }

    return vec3(bestHit.xy - uv, bestHit.z);
}

vec3 ParallaxOcclusionMapping_CalcNormal(sampler2D heightmap, vec2 uv, float scale)
{
    float sx0 = texture(heightmap, uv - vec2(NORMAL_SAMPLE_EPSILON, 0)).r;
    float sx1 = texture(heightmap, uv + vec2(NORMAL_SAMPLE_EPSILON, 0)).r;
    float sy0 = texture(heightmap, uv - vec2(0, NORMAL_SAMPLE_EPSILON)).r;
    float sy1 = texture(heightmap, uv + vec2(0, NORMAL_SAMPLE_EPSILON)).r;

    vec3 v0 = vec3(NORMAL_SAMPLE_EPSILON, 0, (sx1 - sx0) * scale);
    vec3 v1 = vec3(0, NORMAL_SAMPLE_EPSILON, (sy1 - sy0) * scale);

    return cross(v0, v1);
}

float RayHeightIntersect(sampler2D heightmap, vec2 dp, vec2 ds)
{
    float depth_step = 1.0 / RELIEF_LINEAR_STEPS;

    float size = depth_step; // current size of search window
    float depth = 0.0; // current depth position

    // best match found (starts with last position 1.0)
    float best_depth = 1.0;

    // search from front to back for first point inside the object
    for (int i = 0; i < RELIEF_LINEAR_STEPS - 1; i++)
    {
        depth += size;
        float t = texture(heightmap, dp+ds*depth).a;

        if (best_depth > 0.996) // if no depth found yet
            if (depth >= t)
                best_depth = depth; // store best depth
    }

    depth=best_depth;
    // search around first point (depth) for closest match
    for ( int i = 0; i < RELIEF_BINARY_STEPS; i++)
    {
        size*=0.5;
        float t = texture(heightmap, dp+ds*depth).a;

        if (depth >= t)
        {
            best_depth = depth;
            depth -= 2 * size;}
            depth += size;
        }

    return best_depth;
}

vec2 xParallaxOcclusionMappingOffset(sampler2D heightmap, vec2 uv, vec3 rayDir, float scale)
{
    // POM in tangent space
    vec3 rayPos = vec3(uv, 0);
    rayDir *= vec3(1, 1, 1/scale);
    float sampleHeight;

    // Linear search
    float stepSize = 1.0 / float(RELIEF_LINEAR_STEPS);
    for(int i = 1; i < RELIEF_LINEAR_STEPS; i++)
    {
        rayPos += rayDir * stepSize;
        sampleHeight = texture(heightmap, rayPos.xy).a;

        if (rayPos.z + 1.0 < sampleHeight)
            break;
    }

    // Binary search
    vec3 bestHit = rayPos;
    for(int i = 0; i < RELIEF_BINARY_STEPS; i++)
    {
        stepSize *= 0.5;
        sampleHeight = texture(heightmap, rayPos.xy).a;
        if(rayPos.z + 1.0 < sampleHeight)
        {
            bestHit = rayPos;
            rayPos += -rayDir * stepSize;
        }
        else
        rayPos += rayDir * stepSize;
    }

    return bestHit.xy - uv;
}


vec3 ParallaxOcclusionMappingOffset(sampler2D heightmap, vec3 rayPos, vec3 rayDir, float scale)
{
    // POM in tangent space
    vec3 originalPos = rayPos;

    rayDir = normalize(rayDir * vec3(1, 1.0/scale, 1));
    float sampleHeight;

    // Linear search
    float stepSize = 1.0 / float(RELIEF_LINEAR_STEPS);
    for(int i = 1; i < RELIEF_LINEAR_STEPS; i++)
    {
        rayPos += rayDir * stepSize;
        sampleHeight = texture(heightmap, rayPos.xy).x;

        if (rayPos.z + 1.0 < sampleHeight)
            break;
    }

    // Binary search
    vec3 bestHit = rayPos;
    for(int i = 0; i < RELIEF_BINARY_STEPS; i++)
    {
        stepSize *= 0.5;
        sampleHeight = texture(heightmap, rayPos.xy).x;
        if(rayPos.z + 1.0 < sampleHeight)
        {
            bestHit = rayPos;
            rayPos += -rayDir * stepSize;
        }
        else
            rayPos += rayDir * stepSize;
    }

    return bestHit - originalPos;
}