#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/input.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/earlysuspend.h>

#include <asm/atomic.h>
#include <asm/uaccess.h>

#include <linux/slab.h>
#include <linux/semaphore.h>
#include <linux/completion.h>

#if 0

//DECLARE_COMPLETION(op_read_completion);
//DECLARE_COMPLETION(op_erase_completion);
//DECLARE_COMPLETION(op_write_completion);
static struct completion op_read_completion;
static struct completion op_erase_completion;
static struct completion op_write_completion;

#endif

//DECLARE_MUTEX(op_list_mutex); // linux kernel forsake the api.
static struct semaphore semaphore_list_mutex;

#define KOBJECT_DEFAULT_ATTRIBUTE

enum __DEVICE_TYPE__ 
{
	DEVICE_TYPE_START = -1,
	DEVICE_TYPE_LCD,
	DEVICE_TYPE_TP,
	DEVICE_TYPE_FLASH,
	DEVICE_TYPE_CAMERA0,
	DEVICE_TYPE_CAMERA1,
	DEVICE_TYPE_GSENSOR,
	DEVICE_TYPE_MSENSOR,
	DEVICE_TYPE_PSENSOR,
	DEVICE_TYPE_LSENSOR,
	DEVICE_TYPE_END 
};

//Note:device name can't contain spacewhite chars, order base on type

static const char * device_name[]=
{
	"lcd",
	"tp",
	"flash",
	"camera0",
	"camera1",
	"gsensor",
	"msensor",
	"psensor",
	"lsensor",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};


typedef struct __DEVICE_INF__
{
	char    type[64]	;  // lcd,tp,etc
	char	name[64]	;  // ic name
	char	pid[64]		;  // ic pid
	char	vid[64]		;  // ic vid
}DEVICE_INF;

typedef struct __DEVICE_SETS__
{
    struct list_head    entry;
	DEVICE_INF 			device;
}DEVICE_SETS;

LIST_HEAD(device_list_header);

int collector_put_device_info(char * type ,char * name , char * pid , char * vid)
{
	int res = 0;
	DEVICE_SETS * dev_sets_tmp;
	struct list_head * entry;

	if(type == NULL)
		return -1;

	down(&semaphore_list_mutex);

	list_for_each(entry, &device_list_header)
	{
		dev_sets_tmp = list_entry(entry, DEVICE_SETS, entry);	
		if(strcmp(dev_sets_tmp->device.type,type) == 0)
		{
			res = -1;
			goto end;
		}
		// exit if exist.
	}

	DEVICE_SETS * device_sets = kmalloc(sizeof(DEVICE_SETS),GFP_KERNEL);
	if(device_sets == NULL)
	{
			res = -2;
			goto end;
	}

	INIT_LIST_HEAD(&device_sets->entry);

	// above check .
	if(type !=NULL)
		strncpy(device_sets->device.type,type,sizeof(device_sets->device.type)-1);

	if(name !=NULL)
		strncpy(device_sets->device.name,name,sizeof(device_sets->device.name)-1);
	else
		device_sets->device.name[0] = '\0';

	if(pid != NULL)
		strncpy(device_sets->device.pid,pid,sizeof(device_sets->device.pid)-1);
	else
		device_sets->device.pid[0] = '\0';

	if(vid != NULL)
		strncpy(device_sets->device.vid,vid,sizeof(device_sets->device.vid)-1);
	else
		device_sets->device.vid[0] = '\0';

	list_add(&device_sets->entry,&device_list_header);

end:
	up(&semaphore_list_mutex);

	return res;
}
EXPORT_SYMBOL(collector_put_device_info);

static int collector_get_device_info(char * type ,char ** name , char ** pid , char ** vid)
{
	int res = -1;
	DEVICE_SETS * dev_sets_tmp;
	struct list_head * entry;

	down(&semaphore_list_mutex);

	list_for_each(entry, &device_list_header)
	{
		dev_sets_tmp = list_entry(entry, DEVICE_SETS, entry);	
		if(strcmp(dev_sets_tmp->device.type,type) == 0)
		{
			// find it

			if(name != NULL)
				*name = dev_sets_tmp->device.name;

			if(pid != NULL)
				*pid  = dev_sets_tmp->device.pid;

			if(vid != NULL)
				*vid  = dev_sets_tmp->device.vid;
			
			res = 0;
			goto end;
		}
	}
end:
	up(&semaphore_list_mutex);

	return res;
}

static int collector_get_device_type_info(char * node_name,int attr_index,char ** value )
{
	if(strcmp(node_name,device_name[DEVICE_TYPE_LCD]) == 0)
	{
			switch(attr_index)
			{
				case 0:
					collector_get_device_info(device_name[DEVICE_TYPE_LCD],value,NULL,NULL);
					break;
				case 1:
					collector_get_device_info(device_name[DEVICE_TYPE_LCD],NULL,value,NULL);
					break;
				case 2:
					collector_get_device_info(device_name[DEVICE_TYPE_LCD],NULL,NULL,value);
					break;
				default:
					break;
			}
	}
	else if(strcmp(node_name,device_name[DEVICE_TYPE_TP]) == 0)
	{
			switch(attr_index)
			{
				case 0:
					collector_get_device_info(device_name[DEVICE_TYPE_TP],value,NULL,NULL);
					break;
				case 1:
					collector_get_device_info(device_name[DEVICE_TYPE_TP],NULL,value,NULL);
					break;
				case 2:
					collector_get_device_info(device_name[DEVICE_TYPE_TP],NULL,NULL,value);
					break;
				default:
					break;
			}
	}
	else if(strcmp(node_name,device_name[DEVICE_TYPE_FLASH]) == 0)
	{
			switch(attr_index)
			{
				case 0:
					collector_get_device_info(device_name[DEVICE_TYPE_FLASH],value,NULL,NULL);
					break;
				case 1:
					collector_get_device_info(device_name[DEVICE_TYPE_FLASH],NULL,value,NULL);
					break;
				case 2:
					collector_get_device_info(device_name[DEVICE_TYPE_FLASH],NULL,NULL,value);
					break;
				default:
					break;
			}
	}
	else if(strcmp(node_name,device_name[DEVICE_TYPE_CAMERA0]) == 0)
	{
			switch(attr_index)
			{
				case 0:
					collector_get_device_info(device_name[DEVICE_TYPE_CAMERA0],value,NULL,NULL);
					break;
				case 1:
					collector_get_device_info(device_name[DEVICE_TYPE_CAMERA0],NULL,value,NULL);
					break;
				case 2:
					collector_get_device_info(device_name[DEVICE_TYPE_CAMERA0],NULL,NULL,value);
					break;
				default:
					break;
			}
	}
	else if(strcmp(node_name,device_name[DEVICE_TYPE_CAMERA1]) == 0)
	{
			switch(attr_index)
			{
				case 0:
					collector_get_device_info(device_name[DEVICE_TYPE_CAMERA1],value,NULL,NULL);
					break;
				case 1:
					collector_get_device_info(device_name[DEVICE_TYPE_CAMERA1],NULL,value,NULL);
					break;
				case 2:
					collector_get_device_info(device_name[DEVICE_TYPE_CAMERA1],NULL,NULL,value);
					break;
				default:
					break;
			}
	}
	else if(strcmp(node_name,device_name[DEVICE_TYPE_GSENSOR]) == 0)
	{
			switch(attr_index)
			{
				case 0:
					collector_get_device_info(device_name[DEVICE_TYPE_GSENSOR],value,NULL,NULL);
					break;
				case 1:
					collector_get_device_info(device_name[DEVICE_TYPE_GSENSOR],NULL,value,NULL);
					break;
				case 2:
					collector_get_device_info(device_name[DEVICE_TYPE_GSENSOR],NULL,NULL,value);
					break;
				default:
					break;
			}
	}
	else if(strcmp(node_name,device_name[DEVICE_TYPE_MSENSOR]) == 0)
	{
			switch(attr_index)
			{
				case 0:
					collector_get_device_info(device_name[DEVICE_TYPE_MSENSOR],value,NULL,NULL);
					break;
				case 1:
					collector_get_device_info(device_name[DEVICE_TYPE_MSENSOR],NULL,value,NULL);
					break;
				case 2:
					collector_get_device_info(device_name[DEVICE_TYPE_MSENSOR],NULL,NULL,value);
					break;
				default:
					break;
			}
	}
	else if(strcmp(node_name,device_name[DEVICE_TYPE_PSENSOR]) == 0)
	{
			switch(attr_index)
			{
				case 0:
					collector_get_device_info(device_name[DEVICE_TYPE_PSENSOR],value,NULL,NULL);
					break;
				case 1:
					collector_get_device_info(device_name[DEVICE_TYPE_PSENSOR],NULL,value,NULL);
					break;
				case 2:
					collector_get_device_info(device_name[DEVICE_TYPE_PSENSOR],NULL,NULL,value);
					break;
				default:
					break;
			}
	}
	else if(strcmp(node_name,device_name[DEVICE_TYPE_LSENSOR]) == 0)
	{
			switch(attr_index)
			{
				case 0:
					collector_get_device_info(device_name[DEVICE_TYPE_LSENSOR],value,NULL,NULL);
					break;
				case 1:
					collector_get_device_info(device_name[DEVICE_TYPE_LSENSOR],NULL,value,NULL);
					break;
				case 2:
					collector_get_device_info(device_name[DEVICE_TYPE_LSENSOR],NULL,NULL,value);
					break;
				default:
					break;
			}
	}
}

static struct kset *devices_collector_kset;

// Name
static ssize_t device_collector_name_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    int len = 0;
    char *p = buf;
    int i = 0;

	char * node_name = kobj?(kobject_name(kobj)):NULL;

	//p += sprintf(p,"[%s]\n",node_name);
#if 1
	char * value  = NULL ;
	collector_get_device_type_info(node_name,0,&value);
	p += sprintf(p,"%s\n",value);
	len = p - buf;
#else
	char * name = NULL;
	if(strcmp(node_name,device_name[DEVICE_TYPE_LCD]) == 0)
	{
			collector_get_device_info(device_name[DEVICE_TYPE_LCD],&name,NULL,NULL);
			p += sprintf(p,"%s\n",name);
			len = p - buf;
	}
	else if(strcmp(node_name,device_name[DEVICE_TYPE_TP]) == 0)
	{
			collector_get_device_info(device_name[DEVICE_TYPE_TP],&name,NULL,NULL);
			p += sprintf(p,"%s\n",name);
			len = p - buf;
	}
	else if(strcmp(node_name,device_name[DEVICE_TYPE_FLASH]) == 0)
	{
			collector_get_device_info(device_name[DEVICE_TYPE_FLASH],&name,NULL,NULL);
			p += sprintf(p,"%s\n",name);
			len = p - buf;
	}
	else if(strcmp(node_name,device_name[DEVICE_TYPE_CAMERA0]) == 0)
	{
			collector_get_device_info(device_name[DEVICE_TYPE_CAMERA0],&name,NULL,NULL);
			p += sprintf(p,"%s\n",name);
			len = p - buf;
	}
	else if(strcmp(node_name,device_name[DEVICE_TYPE_CAMERA1]) == 0)
	{
			collector_get_device_info(device_name[DEVICE_TYPE_CAMERA1],&name,NULL,NULL);
			p += sprintf(p,"%s\n",name);
			len = p - buf;
	}
	else if(strcmp(node_name,device_name[DEVICE_TYPE_GSENSOR]) == 0)
	{
			collector_get_device_info(device_name[DEVICE_TYPE_GSENSOR],&name,NULL,NULL);
			p += sprintf(p,"%s\n",name);
			len = p - buf;
	}
	else if(strcmp(node_name,device_name[DEVICE_TYPE_MSENSOR]) == 0)
	{
			collector_get_device_info(device_name[DEVICE_TYPE_MSENSOR],&name,NULL,NULL);
			p += sprintf(p,"%s\n",name);
			len = p - buf;
	}
	else if(strcmp(node_name,device_name[DEVICE_TYPE_PSENSOR]) == 0)
	{
			collector_get_device_info(device_name[DEVICE_TYPE_PSENSOR],&name,NULL,NULL);
			p += sprintf(p,"%s\n",name);
			len = p - buf;
	}
	else if(strcmp(node_name,device_name[DEVICE_TYPE_LSENSOR]) == 0)
	{
			collector_get_device_info(device_name[DEVICE_TYPE_LSENSOR],&name,NULL,NULL);
			p += sprintf(p,"%s\n",name);
			len = p - buf;
	}
#endif
	return len;
}

static ssize_t device_collector_name_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t n)
{
	return 0;
}

//
//struct kobj_attribute {
//    struct attribute attr;
//    ssize_t (*show)(struct kobject *kobj, struct kobj_attribute *attr,
//            char *buf);
//    ssize_t (*store)(struct kobject *kobj, struct kobj_attribute *attr,
//             const char *buf, size_t count);
//};
//

static struct kobj_attribute device_collector_name_attr = {   
    .attr = {                                   
        .name = __stringify(name),
        .mode = 0666,                           
    },                                          
    .show  = device_collector_name_show,                       
    .store = device_collector_name_store,                     
};

// vid
static ssize_t device_collector_vid_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    int len = 0;
    char *p = buf;
    int i = 0;


	char * node_name = kobj?(kobject_name(kobj)):NULL;

	//p += sprintf(p,"[%s]\n",node_name);
#if 1
	char * value  = NULL ;
	collector_get_device_type_info(node_name,2,&value);
	p += sprintf(p,"%s\n",value);
	len = p - buf;
#else
	char * vid = NULL;
	if(strcmp(node_name,device_name[DEVICE_TYPE_LCD]) == 0)
	{
			collector_get_device_info(device_name[DEVICE_TYPE_LCD],NULL,NULL,&vid);
			p += sprintf(p,"%s\n",vid);
			len = p - buf;
	}
	else if(strcmp(node_name,device_name[DEVICE_TYPE_TP]) == 0)
	{
			collector_get_device_info(device_name[DEVICE_TYPE_TP],NULL,NULL,&vid);
			p += sprintf(p,"%s\n",vid);
			len = p - buf;
	}
	else if(strcmp(node_name,device_name[DEVICE_TYPE_FLASH]) == 0)
	{
			collector_get_device_info(device_name[DEVICE_TYPE_FLASH],NULL,NULL,&vid);
			p += sprintf(p,"%s\n",vid);
			len = p - buf;
	}
	else if(strcmp(node_name,device_name[DEVICE_TYPE_CAMERA0]) == 0)
	{
			collector_get_device_info(device_name[DEVICE_TYPE_CAMERA0],NULL,NULL,&vid);
			p += sprintf(p,"%s\n",vid);
			len = p - buf;
	}
	else if(strcmp(node_name,device_name[DEVICE_TYPE_CAMERA1]) == 0)
	{
			collector_get_device_info(device_name[DEVICE_TYPE_CAMERA1],NULL,NULL,&vid);
			p += sprintf(p,"%s\n",vid);
			len = p - buf;
	}
	else if(strcmp(node_name,device_name[DEVICE_TYPE_GSENSOR]) == 0)
	{
			collector_get_device_info(device_name[DEVICE_TYPE_GSENSOR],NULL,NULL,&vid);
			p += sprintf(p,"%s\n",vid);
			len = p - buf;
	}
	else if(strcmp(node_name,device_name[DEVICE_TYPE_MSENSOR]) == 0)
	{
			collector_get_device_info(device_name[DEVICE_TYPE_MSENSOR],NULL,NULL,&vid);
			p += sprintf(p,"%s\n",vid);
			len = p - buf;
	}
	else if(strcmp(node_name,device_name[DEVICE_TYPE_PSENSOR]) == 0)
	{
			collector_get_device_info(device_name[DEVICE_TYPE_PSENSOR],NULL,NULL,&vid);
			p += sprintf(p,"%s\n",vid);
			len = p - buf;
	}
	else if(strcmp(node_name,device_name[DEVICE_TYPE_LSENSOR]) == 0)
	{
			collector_get_device_info(device_name[DEVICE_TYPE_LSENSOR],NULL,NULL,&vid);
			p += sprintf(p,"%s\n",vid);
			len = p - buf;
	}
#endif
	return len;
}

static ssize_t device_collector_vid_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t n)
{
	return 0;
}

static struct kobj_attribute device_collector_vid_attr = {   
    .attr = {                                   
        .name = __stringify(vid),
        .mode = 0666,                           
    },                                          
    .show  = device_collector_vid_show,                       
    .store = device_collector_vid_store,                     
};

// pid
static ssize_t device_collector_pid_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    int len = 0;
    char *p = buf;
    int i = 0;


	char * node_name = kobj?(kobject_name(kobj)):NULL;

	//p += sprintf(p,"[%s]\n",node_name);
#if 1
	char * value  = NULL ;
	collector_get_device_type_info(node_name,1,&value);
	p += sprintf(p,"%s\n",value);
	len = p - buf;
#else
	char * pid = NULL;
	if(strcmp(node_name,device_name[DEVICE_TYPE_LCD]) == 0)
	{
			collector_get_device_info(device_name[DEVICE_TYPE_LCD],NULL,&pid,NULL);
			p += sprintf(p,"%s\n",pid);
			len = p - buf;
	}
	else if(strcmp(node_name,device_name[DEVICE_TYPE_TP]) == 0)
	{
			collector_get_device_info(device_name[DEVICE_TYPE_TP],NULL,&pid,NULL);
			p += sprintf(p,"%s\n",pid);
			len = p - buf;
	}
	else if(strcmp(node_name,device_name[DEVICE_TYPE_FLASH]) == 0)
	{
			collector_get_device_info(device_name[DEVICE_TYPE_FLASH],NULL,&pid,NULL);
			p += sprintf(p,"%s\n",pid);
			len = p - buf;
	}
	else if(strcmp(node_name,device_name[DEVICE_TYPE_CAMERA0]) == 0)
	{
			collector_get_device_info(device_name[DEVICE_TYPE_CAMERA0],NULL,&pid,NULL);
			p += sprintf(p,"%s\n",pid);
			len = p - buf;
	}
	else if(strcmp(node_name,device_name[DEVICE_TYPE_CAMERA1]) == 0)
	{
			collector_get_device_info(device_name[DEVICE_TYPE_CAMERA1],NULL,&pid,NULL);
			p += sprintf(p,"%s\n",pid);
			len = p - buf;
	}
	else if(strcmp(node_name,device_name[DEVICE_TYPE_GSENSOR]) == 0)
	{
			collector_get_device_info(device_name[DEVICE_TYPE_GSENSOR],NULL,&pid,NULL);
			p += sprintf(p,"%s\n",pid);
			len = p - buf;
	}
	else if(strcmp(node_name,device_name[DEVICE_TYPE_MSENSOR]) == 0)
	{
			collector_get_device_info(device_name[DEVICE_TYPE_MSENSOR],NULL,&pid,NULL);
			p += sprintf(p,"%s\n",pid);
			len = p - buf;
	}
	else if(strcmp(node_name,device_name[DEVICE_TYPE_PSENSOR]) == 0)
	{
			collector_get_device_info(device_name[DEVICE_TYPE_PSENSOR],NULL,&pid,NULL);
			p += sprintf(p,"%s\n",pid);
			len = p - buf;
	}
	else if(strcmp(node_name,device_name[DEVICE_TYPE_LSENSOR]) == 0)
	{
			collector_get_device_info(device_name[DEVICE_TYPE_LSENSOR],NULL,&pid,NULL);
			p += sprintf(p,"%s\n",pid);
			len = p - buf;
	}
#endif
	return len;
}

static ssize_t device_collector_pid_store(struct kobject *kobj, struct kobj_attribute *attr,const char *buf, size_t n)
{
	return 0;
}

static struct kobj_attribute device_collector_pid_attr = {   
    .attr = {                                   
        .name = __stringify(pid),
        .mode = 0666,                           
    },                                          
    .show  = device_collector_pid_show,                       
    .store = device_collector_pid_store,                     
};

static struct attribute * default_attrs[] = 
{
	&device_collector_name_attr.attr,
	&device_collector_pid_attr.attr,
	&device_collector_vid_attr.attr,
	NULL
};

#if defined(KOBJECT_DEFAULT_ATTRIBUTE)

/* default kobject attribute operations */
static ssize_t device_collector_kobj_attr_show(struct kobject *kobj, struct attribute *attr,
                  char *buf)
{
    struct kobj_attribute *kattr;
    ssize_t ret = -EIO;

    kattr = container_of(attr, struct kobj_attribute, attr);
    if (kattr->show)
        ret = kattr->show(kobj, kattr, buf);
    return ret;
}

static ssize_t device_collector_kobj_attr_store(struct kobject *kobj, struct attribute *attr,
                   const char *buf, size_t count)
{
    struct kobj_attribute *kattr;
    ssize_t ret = -EIO;

    kattr = container_of(attr, struct kobj_attribute, attr);
    if (kattr->store)
        ret = kattr->store(kobj, kattr, buf, count);
    return ret;
}

//struct sysfs_ops {
// ssize_t (*show)(struct kobject *, struct attribute *,char *);
// ssize_t (*store)(struct kobject *,struct attribute *,const char *, size_t);
//};
const struct sysfs_ops device_collector_kobj_sysfs_ops = {
    .show   = device_collector_kobj_attr_show,
    .store  = device_collector_kobj_attr_store,
};

static void device_collector_kset_release(struct kobject *kobj)
{
    struct kset *kset = container_of(kobj, struct kset, kobj);
    pr_debug("kobject: '%s' (%p): %s\n",
         kobject_name(kobj), kobj, __func__);
    kfree(kset);
}

static struct kobj_type device_collector_ktype = {
    .sysfs_ops  = &device_collector_kobj_sysfs_ops,
    .release = device_collector_kset_release,
	.default_attrs = &default_attrs,
};

#endif


int device_collector_add_device(char * dev_name ,struct attribute ** attr)
{

#if defined(KOBJECT_DEFAULT_ATTRIBUTE)

			if(dev_name == NULL)
				return -1;

			int retval;
			struct kobject * devices_kobject = kobject_create();
			if (!devices_kobject)
				 return -ENOMEM;

			devices_kobject->ktype = &device_collector_ktype ; // re-ref

			retval = kobject_add(devices_kobject, &devices_collector_kset->kobj, "%s", dev_name);
		    if (retval)
			{
				kobject_put(devices_kobject);
		        devices_kobject = NULL;
				return -1;
		    }
#else
	if(attr != NULL)
	{
			struct attribute * * attr_entity = (struct attribute **)(attr);

			if(dev_name == NULL)
				return -1;

			struct kobject * devices_kobject = kobject_create_and_add(dev_name,&devices_collector_kset->kobj);
			if (!devices_kobject)
				 return -ENOMEM;

			while( *attr_entity != NULL)
			{
					sysfs_create_file(devices_kobject, *attr_entity);
					attr_entity += 1;			
			}		
			//test attributes
			//sysfs_create_file(devices_kobject,&device_collector_id_attr.attr);
			//sysfs_create_file(devices_kobject,&device_collector_name_attr.attr);
	}	
#endif
	return 0;
}

static int __init device_collector_init(void)
{

	sema_init(&semaphore_list_mutex, 1);

    devices_collector_kset = kset_create_and_add("collector", NULL, NULL);
    if (!devices_collector_kset)
                return -ENOMEM;
#if 1
	// add all support hardware information .
	device_collector_add_device(device_name[DEVICE_TYPE_LCD],		default_attrs);
	device_collector_add_device(device_name[DEVICE_TYPE_TP],		default_attrs);
	device_collector_add_device(device_name[DEVICE_TYPE_FLASH],		default_attrs);
	device_collector_add_device(device_name[DEVICE_TYPE_CAMERA0],	default_attrs);
	device_collector_add_device(device_name[DEVICE_TYPE_CAMERA1],	default_attrs);
	device_collector_add_device(device_name[DEVICE_TYPE_GSENSOR],	default_attrs);
	device_collector_add_device(device_name[DEVICE_TYPE_MSENSOR],	default_attrs);
	device_collector_add_device(device_name[DEVICE_TYPE_PSENSOR],	default_attrs);
	device_collector_add_device(device_name[DEVICE_TYPE_LSENSOR],	default_attrs);
#endif

#if 0
// add data eg:
	collector_put_device_info(device_name[DEVICE_TYPE_LCD] ,	"ili9481" , 	"0x9481" , 		"0x0");
	collector_put_device_info(device_name[DEVICE_TYPE_TP] ,		"ft6206" , 		"0x5206" , 		"0x1");
	collector_put_device_info(device_name[DEVICE_TYPE_FLASH] ,	"emmc xasd" , 	"0x1234" , 		"0x2");
	collector_put_device_info(device_name[DEVICE_TYPE_CAMERA0] ,"ov7670" , 		"0x7670" , 		"0x3");
	collector_put_device_info(device_name[DEVICE_TYPE_CAMERA1] ,"gc0308" , 		"0x0308" , 		"0x4");
	collector_put_device_info(device_name[DEVICE_TYPE_GSENSOR] ,"mma8453q" , 	"0x8453" , 		"0x5");
	collector_put_device_info(device_name[DEVICE_TYPE_MSENSOR] ,"mpu3000" , 	"0x3000" , 		"0x6");
	collector_put_device_info(device_name[DEVICE_TYPE_PSENSOR] ,"cm3623" , 		"0x3623" , 		"0x7");
	collector_put_device_info(device_name[DEVICE_TYPE_LSENSOR] ,"cm3623" , 		"0x3624" , 		"0x8");
#endif



/*
	init_completion(&op_read_completion);	    //init
	init_completion(&op_erase_completion);     //init
	init_completion(&op_write_completion);    //init

*/
    return 0;
}

/* should never be called */
static void __exit device_collector_exit(void)
{

}

module_init(device_collector_init);
//subsys_initcall(device_collector_init);
module_exit(device_collector_exit);

MODULE_AUTHOR("huyanwei <huyanwei@sunmontech.com>");
MODULE_DESCRIPTION("Current Hardware Devices Information.");
MODULE_LICENSE("GPL");
