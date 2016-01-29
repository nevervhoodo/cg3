#define ANTIAL 2
#define ANTIAL2 4
#define PRECISION 0.000001
#define NOCHANGE 0.01
#define GCONST 6.67384e-11
#define VC 3.0e+8
#define MAGICMAXSIZE 65632
#define MAGICMAXSIZE2 1.723025e+13

struct SRay
{
  double3 m_start;
  double3 m_dir;
};

double3 MakeSky (double3 ray_pos, double alpha,
	__global uint4* stars, const uint height,
	const uint width)
{
    if (length(ray_pos)>PRECISION)
        ray_pos = normalize(ray_pos);
    else
        printf("sky\n");;
    double dphi,dteta;
    uint phi, teta;
    if (length(ray_pos)>PRECISION)
        ray_pos = normalize(ray_pos);
    else
    	printf("tracer.cpp\n");
    dphi = (atan2(ray_pos.x, ray_pos.y)+M_PI)/M_PI/2*(width-1);
    dteta = (asin (ray_pos.z)+M_PI/2)/M_PI*(height-1);
    phi = dphi;
    teta = dteta;


    uchar r = stars[(teta*width+phi)].x;
    uchar g = stars[(teta*width+phi)].y;
    uchar b = stars[(teta*width+phi)].z;

    return (double3)(r,g,b) * (double)(alpha) / 255.0;
}

double FoundDisk(double3 m_start, double3 m_dir, double3 *color, 
	double *alpha, __global double *dtime, __global double *diskrad,
	__global double *raddisk, __global uint4 *disk,
	const uint height, const uint width)
{
        double dt = -(m_start.z/m_dir.z);
        double rad;
        double2 coord;
        uint i,j;
        if ((dt>PRECISION)&&(dt<=*dtime))
        {
                m_start += m_dir*dt;
                rad = sqrt(m_start.x*m_start.x + 
                        m_start.y*m_start.y); 
                if (rad <= *raddisk )
                {

                        coord = (double2)(m_start.x,m_start.y);
                        coord /= *raddisk;
                        coord *= *diskrad;
                        i = coord.y + *diskrad;
                        j = coord.x + *diskrad;
                        uint a = disk[i*width+j].w;
                        
                        (*color).x = (double)a * (double)disk[i*width+j].x * (*alpha) / 255.0/255.0;
                        (*color).y = (double)a * (double)disk[i*width+j].y * (*alpha) / 255.0/255.0;
                        (*color).z = (double)a * (double)disk[i*width+j].z * (*alpha) / 255.0/255.0;
                        *alpha = (*alpha) * (1 - a/255.0);
                }
        }
        return -1;
}

double BlackHole(double3 m_start, double3 m_dir, 
	__global double *dtime, __global double *radhole)
{
        double r1,r2;
        double3 m_fin = m_start + m_dir;
        r1 = sqrt(m_start.x*m_start.x + m_start.y*m_start.y 
                + m_start.z*m_start.z);
        
        r2 = sqrt(m_fin.x*m_fin.x + m_fin.y*m_fin.y + m_fin.z*m_fin.z); 

        if (((r1>*radhole)^(r2>*radhole))||(r2<*radhole))
        {
                //cout<<"black here"<<endl;
                return *dtime/2.0;
        }
        else
                return -1;  
}


void MakeRay(int i, int j, struct SRay *ray, uint c, 
	const double3 m_pos, const double3 m_forward, 
	const double3 m_right,
	const double3 m_up, const double2 m_viewAngle, 
	const double2 m_resolution)
{
        double ppx = i - m_resolution.x / 2.0;
        double ppy = j - m_resolution.y / 2.0;
        double3 n_forw, n_up, n_right;
        n_up = normalize(m_up);
        n_right = normalize(m_right);
        uint tmp = c/ANTIAL;
        float d = (float)(ANTIAL);
        d = 1/d;
        float i2 = d/2.0+tmp*d;
        tmp = c%ANTIAL;
        float j2 = d/2.0+tmp*d;
        double x = 2.0 * length(m_forward) * 
            tan(m_viewAngle.x) / m_resolution.x;
        double y = 2.0 * length(m_forward) * 
            tan(m_viewAngle.y) / m_resolution.y;
        x *= j2 + ppx;
        y *= i2 + ppy;
		ray->m_start = m_forward;
        ray->m_start += x * n_right;
        ray->m_start += y * n_up;
        ray->m_dir = normalize(ray->m_start);
        ray->m_start += m_pos;
}

double3 TraceRay(double3 m_dir, double3 m_start, double3 m_forward,
	__global double *coeff, __global double *dtime,
	__global uint4* stars, const uint stars_height,
	const uint stars_width, __global uint4 *disk, 
	const uint disk_height, const uint disk_width,
	__global double *radhole, __global double *diskrad,
	__global double *raddisk)
{
	double r, ht, dt;
    bool early = true;
    double alpha = 1;
    double3 a,an,change;
    double3 color = (double3)(0.0,0.0,0.0);
    if (length(m_dir)>PRECISION)
        m_dir = normalize(m_dir);
    else
        printf("tracer0\n");
    m_dir *= VC;
    long int kmax = 1000;

    for (;;)
    {
        r = length(m_start);
        a = -(*coeff)/r/r/r * m_start;
        an = a - dot(a,m_dir)/dot(m_dir,m_dir)*m_dir;
        change = (*dtime)*m_dir+an*(*dtime)*(*dtime)/2.0;
        if (early)
            if (dot(m_start,m_forward)>PRECISION)
                early = false;
        if ((!early)&&(length((*dtime)*an)<10000))
        {

                color += MakeSky(m_dir,alpha,stars,stars_height,
                	stars_width);
                return color;
        }
        m_start+=change;
        m_dir+=(*dtime)*an;
        if (length(m_dir)>PRECISION)
            m_dir = normalize(m_dir);
        else
            printf("tracer\n");
        m_dir *= VC;
        ht = BlackHole(m_start,m_dir,dtime, radhole);
        dt = FoundDisk(m_start,m_dir,&color,&alpha,dtime,diskrad,
        	raddisk,disk,disk_height,disk_width);

        if ((dt>PRECISION)&&(dt<*dtime)&&(alpha<PRECISION))
        {
                //kmax = k;
                return color;
        }
        if ((ht>PRECISION)&&(ht<*dtime))
        {
                //kmax = k;
                return color;
        }
    }
    return (0.0,0.0,0.0);
}

__kernel void test(__global double3 *pixels, const int xRes, 
 	const int yRes, __global double* info,
 	__global uint4 *stars, const uint stars_height, 
 	const uint stars_width, __global uint4 *disk, 
 	const uint disk_height, const uint disk_width)
 {
	__global double *mass = &(info[0]);
	__global double *raddisk = &(info[1]);
	__global double *radhole = &(info[2]);
	__global double *coeff = &(info[3]);
	__global double *dtime = &(info[4]);
    __global double *diskrad = &(info[5]);
    const double3 m_pos = (double3)(info[6],info[7],info[8]);
    const double3 m_forward = (double3)(info[9],info[10],info[11]);
	const double3 m_right = (double3)(info[12],info[13],info[14]);
	const double3 m_up = (double3)(info[15],info[16],info[17]);
	const double2 m_viewAngle = (double2)(info[18],info[19]);

	const double disksize = info[20];
	const double2 m_resolution = (double2)(info[21],info[22]);
	int gid = get_global_id(0);
	pixels[gid] = (double3)(0.0,0.0,0.0);

	struct SRay ray;
	int j = gid/xRes;
	int i = gid%yRes;
	
	pixels[gid] = (double3)(0.0,0.0,0.0);
	for (uint c=0;c<ANTIAL2;c++)
	{
		MakeRay(i,j,&ray,c,m_pos,m_forward,m_right,m_up,
			m_viewAngle,m_resolution);
		//pixels[gid] += ray.m_dir;
		pixels[gid] += TraceRay(ray.m_dir,ray.m_start,m_forward,coeff,dtime,
			stars,stars_height,stars_width, disk, disk_height, disk_width,
			radhole, diskrad, raddisk);
	}

	pixels[gid] /= ANTIAL2; 

 }