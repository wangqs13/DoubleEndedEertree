#pragma once

#include <cassert>
#include <array>
#include <set>
#include <queue>
#include <vector>

template<int alpha>
struct DoubleEndedEertreeByOccurrenceRecording
{
	struct node
	{
		std::array<node*, alpha> next;
		std::array<node*, alpha> dlink;
		node* link, * prev;
		int len, linkcnt;
		std::set<int> s;
		node() : next{}, dlink{}, prev(nullptr), link(nullptr), len(0), linkcnt(0), s() { }
		void set_link(node* to)
		{
			if (link) link->linkcnt -= 1;
			link = to;
			if (link) link->linkcnt += 1;
		}
	};
	struct cmp_node_len
	{
		bool operator () (node* a, node* b) const
		{
			return a->len < b->len || a->len == b->len && a < b;
		};
	};
	int node_cnt;
	node* new_node(int len)
	{
		node_cnt += 1;
		node* it = new node();
		it->len = len;
		return it;
	}
	std::vector<node*> to_del;
	void del_node(node* v)
	{
		node_cnt -= 1;
		v->link->linkcnt -= 1;
		to_del.push_back(v);
	}
	int start_pos, end_pos;
	node* even, * odd;
	std::deque<int> data;
	std::deque<std::set<node*, cmp_node_len>> prenode, sufnode;
	auto& get_data(int pos)
	{
		assert(start_pos <= pos && pos < end_pos);
		return data[pos - start_pos];
	}
	auto& get_prenode(int pos)
	{
		assert(start_pos <= pos && pos < end_pos);
		return prenode[pos - start_pos];
	}
	auto& get_sufnode(int pos)
	{
		assert(start_pos <= pos && pos < end_pos);
		return sufnode[pos - start_pos];
	}
	bool unique(node* v)
	{
		return v->linkcnt == 0 && v->s.size() == 1;
	}
	node* get_prepal()
	{
		return !prenode.empty() && !prenode.front().empty() ? *prenode.front().rbegin() : even;
	}
	node* get_sufpal()
	{
		return !sufnode.empty() && !sufnode.back().empty() ? *sufnode.back().rbegin() : even;
	}
	DoubleEndedEertreeByOccurrenceRecording()
	{
		node_cnt = 0;
		start_pos = end_pos = 0;
		odd = new_node(-1);
		odd->set_link(odd);
		even = new_node(0);
		even->set_link(odd);
		for (int x = 0; x < alpha; ++x)
			odd->dlink[x] = even->dlink[x] = odd;
	}
	void push_back(int x)
	{
		node* v = get_sufpal();
		data.push_back(x);
		prenode.emplace_back();
		sufnode.emplace_back();
		end_pos += 1;
		auto get_suf_link = [&](node* v, int x) -> node*
		{
			return end_pos - 1 - v->len - 1 >= start_pos && get_data(end_pos - 1 - v->len - 1) == x ? v : v->dlink[x];
		};
		node* sufpal = get_suf_link(v, x);
		if (!sufpal->next[x])
		{
			node* tmp = new_node(sufpal->len + 2);
			tmp->prev = sufpal;
			node* tmp_link = get_suf_link(sufpal->link, x)->next[x];
			if (!tmp_link) tmp_link = even;
			tmp->set_link(tmp_link);
			tmp->dlink = tmp->link->dlink;
			tmp->dlink[get_data(end_pos - 1 - tmp->link->len)] = tmp->link;
			sufpal->next[x] = tmp;
		}
		node* vp = sufpal->next[x];
		vp->s.insert(end_pos - vp->len);
		get_prenode(end_pos - vp->len).insert(vp);
		get_sufnode(end_pos - 1).insert(vp);
	}
	void push_front(int x)
	{
		node* v = get_prepal();
		data.push_front(x);
		prenode.emplace_front();
		sufnode.emplace_front();
		start_pos -= 1;
		auto get_pre_link = [&](node* v, int x) -> node*
		{
			return start_pos + v->len + 1 < end_pos && get_data(start_pos + v->len + 1) == x ? v : v->dlink[x];
		};
		node* prepal = get_pre_link(v, x);
		if (!prepal->next[x])
		{
			node* tmp = new_node(prepal->len + 2);
			tmp->prev = prepal;
			node* tmp_link = get_pre_link(prepal->link, x)->next[x];
			if (!tmp_link) tmp_link = even;
			tmp->set_link(tmp_link);
			tmp->dlink = tmp->link->dlink;
			tmp->dlink[get_data(start_pos + tmp->link->len)] = tmp->link;
			prepal->next[x] = tmp;
		}
		node* vp = prepal->next[x];
		vp->s.insert(start_pos);
		get_prenode(start_pos).insert(vp);
		get_sufnode(start_pos + vp->len - 1).insert(vp);
	}
	void pop_back()
	{
		int x = data.back();
		node* v = get_sufpal();
		if (unique(v))
		{
			v->prev->next[x] = nullptr;
			del_node(v);
		}
		for (auto u : sufnode.back())
		{
			u->s.erase(end_pos - u->len);
			get_prenode(end_pos - u->len).erase(u);
		}
		if (v->link->len >= 1)
		{
			v->link->s.insert(end_pos - v->len);
			get_prenode(end_pos - v->len).insert(v->link);
			get_sufnode(end_pos - v->len + v->link->len - 1).insert(v->link);
		}
		end_pos -= 1;
		data.pop_back();
		prenode.pop_back();
		sufnode.pop_back();
	}
	void pop_front()
	{
		int x = data.front();
		node* v = get_prepal();
		if (unique(v))
		{
			v->prev->next[x] = nullptr;
			del_node(v);
		}
		for (auto u : prenode.front())
		{
			u->s.erase(start_pos);
			get_sufnode(start_pos + u->len - 1).erase(u);
		}
		if (v->link->len >= 1)
		{
			v->link->s.insert(start_pos + v->len - v->link->len);
			get_prenode(start_pos + v->len - v->link->len).insert(v->link);
			get_sufnode(start_pos + v->len - 1).insert(v->link);
		}
		start_pos += 1;
		data.pop_front();
		prenode.pop_front();
		sufnode.pop_front();
	}
	int count_palindrome()
	{
		return node_cnt - 2;
	}
};

template<int alpha>
struct DoubleEndedEertreeBySurfaceRecording
{
	struct node
	{
		std::array<node*, alpha> next;
		std::array<node*, alpha> dlink;
		node* link, * prev;
		int len, linkcnt, cnt;
		node() : next{}, dlink{}, prev(nullptr), link(nullptr), len(0), linkcnt(0), cnt(0) { }
		void set_link(node* to)
		{
			if (link) link->linkcnt -= 1;
			link = to;
			if (link) link->linkcnt += 1;
		}
	};
	int node_cnt;
	node* new_node(int len)
	{
		node_cnt += 1;
		node* it = new node();
		it->len = len;
		return it;
	}
	std::vector<node*> to_del;
	void del_node(node* v)
	{
		node_cnt -= 1;
		v->link->linkcnt -= 1;
		to_del.push_back(v);
	}
	int start_pos, end_pos;
	node* even, * odd;
	std::deque<int> data;
	std::deque<node*> presurf, sufsurf;
	auto& get_data(int pos)
	{
		assert(start_pos <= pos && pos < end_pos);
		return data[pos - start_pos];
	}
	auto& get_presurf(int pos)
	{
		assert(start_pos <= pos && pos < end_pos);
		return presurf[pos - start_pos];
	}
	auto& get_sufsurf(int pos)
	{
		assert(start_pos <= pos && pos < end_pos);
		return sufsurf[pos - start_pos];
	}
	bool unique(node* v)
	{
		return v->linkcnt == 0 && v->cnt == 1;
	}
	node* get_prepal()
	{
		return !presurf.empty() ? presurf.front() : even;
	}
	node* get_sufpal()
	{
		return !sufsurf.empty() ? sufsurf.back() : even;
	}
	DoubleEndedEertreeBySurfaceRecording()
	{
		node_cnt = 0;
		start_pos = end_pos = 0;
		odd = new_node(-1);
		odd->set_link(odd);
		even = new_node(0);
		even->set_link(odd);
		for (int x = 0; x < alpha; ++x)
			odd->dlink[x] = even->dlink[x] = odd;
	}
	void push_back(int x)
	{
		node* v = get_sufpal();
		data.push_back(x);
		presurf.push_back(even);
		sufsurf.push_back(even);
		end_pos += 1;
		auto get_suf_link = [&](node* v, int x) -> node*
		{
			return end_pos - 1 - v->len - 1 >= start_pos && get_data(end_pos - 1 - v->len - 1) == x ? v : v->dlink[x];
		};
		node* sufpal = get_suf_link(v, x);
		if (!sufpal->next[x])
		{
			node* tmp = new_node(sufpal->len + 2);
			tmp->prev = sufpal;
			node* tmp_link = get_suf_link(sufpal->link, x)->next[x];
			if (!tmp_link) tmp_link = even;
			tmp->set_link(tmp_link);
			tmp->dlink = tmp->link->dlink;
			tmp->dlink[get_data(end_pos - 1 - tmp->link->len)] = tmp->link;
			sufpal->next[x] = tmp;
		}
		node* vp = sufpal->next[x];
		get_presurf(end_pos - 1) = even;
		get_presurf(end_pos - vp->len) = vp;
		get_sufsurf(end_pos - 1) = vp;
		if (vp->link->len >= 1 && get_sufsurf(end_pos - vp->len + vp->link->len - 1) == vp->link)
			get_sufsurf(end_pos - vp->len + vp->link->len - 1) = even;
		vp->cnt += 1;
	}
	void push_front(int x)
	{
		node* v = get_prepal();
		data.push_front(x);
		presurf.push_front(even);
		sufsurf.push_front(even);
		start_pos -= 1;
		auto get_pre_link = [&](node* v, int x) -> node*
		{
			return start_pos + v->len + 1 < end_pos && get_data(start_pos + v->len + 1) == x ? v : v->dlink[x];
		};
		node* prepal = get_pre_link(v, x);
		if (!prepal->next[x])
		{
			node* tmp = new_node(prepal->len + 2);
			tmp->prev = prepal;
			node* tmp_link = get_pre_link(prepal->link, x)->next[x];
			if (!tmp_link) tmp_link = even;
			tmp->set_link(tmp_link);
			tmp->dlink = tmp->link->dlink;
			tmp->dlink[get_data(start_pos + tmp->link->len)] = tmp->link;
			prepal->next[x] = tmp;
		}
		node* vp = prepal->next[x];
		get_presurf(start_pos) = vp;
		get_sufsurf(start_pos) = even;
		get_sufsurf(start_pos + vp->len - 1) = vp;
		if (vp->link->len >= 1 && get_presurf(start_pos + vp->len - vp->link->len) == vp->link)
			get_presurf(start_pos + vp->len - vp->link->len) = even;
		vp->cnt += 1;
	}
	void pop_back()
	{
		int x = data.back();
		node* v = get_sufpal();
		if (unique(v))
		{
			v->prev->next[x] = nullptr;
			del_node(v);
		}
		v->cnt -= 1;
		if (v->link->len >= 1 && get_sufsurf(end_pos - v->len + v->link->len - 1)->len < v->link->len)
		{
			get_sufsurf(end_pos - v->len + v->link->len - 1) = v->link;
			get_presurf(end_pos - v->len) = v->link;
		}
		else
			get_presurf(end_pos - v->len) = even;
		end_pos -= 1;
		data.pop_back();
		presurf.pop_back();
		sufsurf.pop_back();
	}
	void pop_front()
	{
		int x = data.front();
		node* v = get_prepal();
		if (unique(v))
		{
			v->prev->next[x] = nullptr;
			del_node(v);
		}
		v->cnt -= 1;
		if (v->link->len >= 1 && get_presurf(start_pos + v->len - v->link->len)->len < v->link->len)
		{
			get_presurf(start_pos + v->len - v->link->len) = v->link;
			get_sufsurf(start_pos + v->len - 1) = v->link;
		}
		else
			get_sufsurf(start_pos + v->len - 1) = even;
		start_pos += 1;
		data.pop_front();
		presurf.pop_front();
		sufsurf.pop_front();
	}
	int count_palindrome()
	{
		return node_cnt - 2;
	}
};
