
struct node {
	char data[8];
	struct node *link;
};
// ����ü�� �ݵ�� ������Ͽ� ����


void insert_node_head(struct node *np);
void insert_node_tail(struct node *np);
void insert_node_ascn(struct node *np);
void free_node(struct node *np);
void tour_list();
void free_list();

struct node *get_node();
